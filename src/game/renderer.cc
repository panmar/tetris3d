#include "game/renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace {

const std::string solid_vs = R"~(
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * world * vec4(position, 1.0);
}

)~";

const std::string solid_fs = R"~(
#version 330 core
out vec4 finalcolor;

uniform vec4 color;

void main() {
    finalcolor = color;
}

)~";

const std::string solid_wire_vs = R"~(

#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * world * vec4(position, 1.0);
}

)~";

const std::string solid_wire_fs = R"~(

#version 330 core
out vec4 final_color;

uniform vec4 color;

void main() {
    final_color = color;
}

)~";

const std::string refract_vs = R"~(

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 Normal;
out vec3 Position;

uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;

void main() {
    Normal = mat3(transpose(inverse(world))) * aNormal;
    Position = vec3(world * vec4(aPos, 1.0));
    gl_Position = projection * view * world * vec4(aPos, 1.0);
}

)~";

const std::string refract_fs = R"~(

#version 330 core
out vec4 final_color;

in vec3 Normal;
in vec3 Position;

uniform vec3 camera_pos;
uniform samplerCube skybox;
uniform vec4 color;

void main() {             
    vec3 I = normalize(Position - camera_pos);
    vec3 R = reflect(I, normalize(Normal));
    vec4 texture_color = vec4(texture(skybox, R).rgb, color.a);
    vec4 mixed_color = mix(color, texture_color, 0.4);
    final_color = mixed_color;
}

)~";

const std::string skybox_vs = R"~(

#version 330 core
layout (location = 0) in vec3 position;

out vec3 tex_coords;

uniform mat4 projection;
uniform mat4 view;

void main() {
    tex_coords = position;
    vec4 pos = projection * view * vec4(position, 1.0);
    gl_Position = pos.xyww;
}

)~";

const std::string skybox_fs = R"~(

#version 330 core
out vec4 final_color;

in vec3 tex_coords;

uniform samplerCube skybox;
uniform vec4 color;

void main() {    
    final_color = color * texture(skybox, tex_coords) * 0.75;
}

)~";

} // namespace

namespace tetris3d {

BasicRenderer::BasicRenderer(Config& config) : config_(config) {}

void BasicRenderer::Render(const GameState& state,
                           const PerspectiveCamera& camera) {
    glViewport(0, 0, framebuffer_width_, framebuffer_height_);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(glm::degrees(camera.GetFov()), camera.GetAspectRatio(),
                   camera.GetNear(), camera.GetFar());
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camera.GetPosition().x, camera.GetPosition().y,
              camera.GetPosition().z, camera.GetTarget().x,
              camera.GetTarget().y, camera.GetTarget().z, camera.GetUp().x,
              camera.GetUp().y, camera.GetUp().z);

    if (state.phase == GameState::Phase::Lost) {
        glClearColor(0.2f, 0.1f, 0.1f, 1.f);
    } else if (state.phase == GameState::Phase::LayersErase) {
        glClearColor(0.9f, 0.9f, 0.9f, 1.f);
    } else if (state.paused) {
        glClearColor(0.05f, 0.08f, 0.08f, 1.f);
    } else {
        glClearColor(0.2f, 0.2f, 0.2f, 1.f);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    if (config_.graphics_multisampling) {
        glEnable(GL_MULTISAMPLE);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderBounds(state.board, camera.GetForward());

    RenderBoard(state.board);

    RenderFallingBlock(state.falling_block);
    if (state.phase == GameState::Phase::BlockFalling) {
        RenderFallingBlockProjection(state);
    }
}

void BasicRenderer::RenderBoard(const Board& board) {
    for (size_t k = 0; k < board.GetHeight(); ++k) {
        for (size_t i = 0; i < board.GetWidth(); ++i) {
            for (size_t j = 0; j < board.GetDepth(); ++j) {
                auto index = (i * board.GetWidth() + j) +
                             (k * board.GetWidth() * board.GetDepth());
                if (board.GetCell(index)) {
                    auto position = glm::vec3(i, k, j);
                    Color color = FromPackedColorToColor(board.GetCell(index));
                    RenderTetrisCube(position + glm::vec3(0.5f, 0.5f, 0.5f),
                                     color);
                }
            }
        }
    }
}

void BasicRenderer::RenderFallingBlock(const Block& block) {
    Color color;
    color.r = block.GetColor().r / 255.f;
    color.g = block.GetColor().g / 255.f;
    color.b = block.GetColor().b / 255.f;

    for (auto& offset : block.GetCubeOffsets()) {
        RenderTetrisCube(glm::vec3(block.GetPosition() + offset) +
                             glm::vec3(0.5f, 0.5f, 0.5f),
                         color);
    }
}

void BasicRenderer::RenderFallingBlockProjection(const GameState& state) {
    auto projected_block = state.falling_block;
    while (projected_block.IsValid(state.board)) {
        projected_block.Translate(glm::ivec3(0, -1, 0));
    }
    projected_block.Translate(glm::ivec3(0, 1, 0));

    Color color{0.9, 0.9f, 0.9f, 0.6f};

    glColorMask(false, false, false, false);
    for (auto& offset : projected_block.GetCubeOffsets()) {
        RenderCube(glm::vec3(projected_block.GetPosition() + offset) +
                       glm::vec3(0.5f, 0.5f, 0.5f),
                   color, 1.f, 1.f, 1.f);
    }

    glColorMask(true, true, true, true);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (auto& offset : projected_block.GetCubeOffsets()) {
        RenderCube(glm::vec3(projected_block.GetPosition() + offset) +
                       glm::vec3(0.5f, 0.5f, 0.5f),
                   color, 1.f, 1.f, 1.f);
    }
    glDisable(GL_BLEND);
    glDepthFunc(GL_LESS);
}

void BasicRenderer::RenderGizmo(const glm::vec3& position) {
    glBegin(GL_LINES);

    auto edge = 1.f;

    glColor3f(1.f, 0.f, 0.f);
    glVertex3f(position.x, position.y, position.z);
    glVertex3f(position.x + edge, position.y, position.z);

    glColor3f(0.f, 1.f, 0.f);
    glVertex3f(position.x, position.y, position.z);
    glVertex3f(position.x, position.y + edge, position.z);

    glColor3f(0.f, 0.f, 1.f);
    glVertex3f(position.x, position.y, position.z);
    glVertex3f(position.x, position.y, position.z + edge);

    glEnd();
}

void BasicRenderer::RenderBounds(const Board& board,
                                 const glm::vec3& view_dir) {
    auto width = board.GetWidth();
    auto height = board.GetHeight();
    auto depth = board.GetDepth();

    glBegin(GL_LINES);

    glColor3f(0.25f, 0.25f, 0.25f);

    // floor rendering
    for (auto i = 0.f; i < width + 0.1f; i += 1.f) {
        glVertex3f(i, 0.f, 0.f);
        glVertex3f(i, 0.f, depth);
        glVertex3f(0.f, 0.f, i);
        glVertex3f(width, 0.f, i);
    }

    {
        glm::vec3 wall_dir{0, 0, 1};
        if (glm::dot(view_dir, wall_dir) < 0.f) {
            for (auto i = 0.f; i < width + 0.1f; i += 1.f) {
                glVertex3f(i, 0.f, 0.f);
                glVertex3f(i, height, 0.f);
            }

            for (auto i = 1.f; i < height + 0.1f; i += 1.f) {
                glVertex3f(0.f, i, 0.f);
                glVertex3f(width, i, 0.f);
            }
        }
    }

    {
        glm::vec3 wall_dir{1, 0, 0};
        if (glm::dot(view_dir, wall_dir) < 0.f) {
            for (auto i = 0.f; i < width + 0.1f; i += 1.f) {
                glVertex3f(0.f, 0.f, i);
                glVertex3f(0.f, height, i);
            }

            for (auto i = 1.f; i < height + 0.1f; i += 1.f) {
                glVertex3f(0.f, i, 0.f);
                glVertex3f(0.f, i, depth);
            }
        }
    }

    {
        glm::vec3 wall_dir{0, 0, -1};
        if (glm::dot(view_dir, wall_dir) < 0.f) {
            for (auto i = 0.f; i < width + 0.1f; i += 1.f) {
                glVertex3f(i, 0.f, depth);
                glVertex3f(i, height, depth);
            }

            for (auto i = 1.f; i < height + 0.1f; i += 1.f) {
                glVertex3f(width, i, depth);
                glVertex3f(0.f, i, depth);
            }
        }
    }

    {
        glm::vec3 wall_dir{-1, 0, 0};
        if (glm::dot(view_dir, wall_dir) < 0.f) {
            for (auto i = 0.f; i < width + 0.1f; i += 1.f) {
                glVertex3f(width, 0.f, i);
                glVertex3f(width, height, i);
            }

            for (auto i = 1.f; i < height + 0.1f; i += 1.f) {
                glVertex3f(width, i, depth);
                glVertex3f(width, i, 0.f);
            }
        }
    }

    glEnd();
}

void BasicRenderer::RenderCube(const glm::vec3& position, const Color& color,
                               float width, float depth, float height) {
    glBegin(GL_TRIANGLES);
    glColor4f(color.r, color.g, color.b, color.a);

    auto wOver2 = width / 2.f;
    auto hOver2 = height / 2.f;
    auto dOver2 = depth / 2.f;
    glm::vec3 positions[] = {
        {-wOver2, -hOver2, -dOver2}, // 0
        {-wOver2, hOver2, -dOver2},  // 1
        {wOver2, hOver2, -dOver2},   // 2
        {-wOver2, -hOver2, -dOver2}, {wOver2, hOver2, -dOver2},
        {wOver2, -hOver2, -dOver2}, // 3
        {wOver2, -hOver2, -dOver2},  {wOver2, hOver2, -dOver2},
        {wOver2, -hOver2, dOver2}, // 4
        {wOver2, -hOver2, dOver2},   {wOver2, hOver2, -dOver2},
        {wOver2, hOver2, dOver2}, // 5
        {wOver2, hOver2, -dOver2},   {-wOver2, hOver2, -dOver2},
        {-wOver2, hOver2, dOver2}, // 6
        {wOver2, hOver2, -dOver2},   {-wOver2, hOver2, dOver2},
        {wOver2, hOver2, dOver2},    {wOver2, -hOver2, dOver2},
        {-wOver2, hOver2, dOver2},   {-wOver2, -hOver2, dOver2}, // 7
        {wOver2, -hOver2, dOver2},   {wOver2, hOver2, dOver2},
        {-wOver2, hOver2, dOver2},   {-wOver2, -hOver2, dOver2},
        {-wOver2, hOver2, -dOver2},  {-wOver2, -hOver2, -dOver2},
        {-wOver2, -hOver2, dOver2},  {-wOver2, hOver2, dOver2},
        {-wOver2, hOver2, -dOver2},  {-wOver2, -hOver2, -dOver2},
        {wOver2, -hOver2, -dOver2},  {-wOver2, -hOver2, dOver2},
        {wOver2, -hOver2, -dOver2},  {wOver2, -hOver2, dOver2},
        {-wOver2, -hOver2, dOver2},
    };

    for (auto& pos : positions) {
        pos += position;
    }

    auto positions_count = sizeof(positions) / sizeof(positions[0]);

    for (size_t i = 0; i < positions_count; ++i) {
        glVertex3f(positions[i].x, positions[i].y, positions[i].z);
    }
    glEnd();
}

void BasicRenderer::RenderTetrisCube(const glm::vec3& pos, const Color& color) {
    Color cube_edge_color{0.1f, 0.1f, 0.1f};
    RenderCube(pos, cube_edge_color, 1.0f, 1.0f, 1.0f);
    RenderCube(pos, color, 1.01f, 0.8f, 0.8f);
    RenderCube(pos, color, 0.8f, 1.01f, 0.8f);
    RenderCube(pos, color, 0.8f, 0.8f, 1.01f);
}

// -----------------------------------------------------------------------
// -----------------------------------------------------------------------

Shader::~Shader() {
    if (id_) {
        glDeleteShader(id_);
    }
}

void Shader::Create(const std::string& vs_str, const std::string& fs_str) {
    unsigned vs = glCreateShader(GL_VERTEX_SHADER);
    const char* vs_cstr = vs_str.c_str();
    glShaderSource(vs, 1, &vs_cstr, nullptr);
    glCompileShader(vs);
    LogShaderErrorsIfAny(vs);
    unsigned fs = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fs_cstr = fs_str.c_str();
    glShaderSource(fs, 1, &fs_cstr, nullptr);
    glCompileShader(fs);
    LogShaderErrorsIfAny(fs);
    this->id_ = glCreateProgram();
    glAttachShader(id_, vs);
    glAttachShader(id_, fs);
    glLinkProgram(id_);
    LogProgramErrorsIfAny(id_);
    glDeleteShader(vs);
    glDeleteShader(fs);
}

void Shader::SetParam(const char* name, bool value) const {
    glUniform1i(glGetUniformLocation(id_, name), static_cast<int>(value));
}

void Shader::SetParam(const char* name, int value) const {
    glUniform1i(glGetUniformLocation(id_, name), value);
}

void Shader::SetParam(const char* name, float value) const {
    glUniform1f(glGetUniformLocation(id_, name), value);
}

void Shader::SetParam(const char* name, const glm::vec2& value) const {
    glUniform2fv(glGetUniformLocation(id_, name), 1, &value[0]);
}

void Shader::SetParam(const char* name, float x, float y) const {
    glUniform2f(glGetUniformLocation(id_, name), x, y);
}

void Shader::SetParam(const char* name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(id_, name), 1, &value[0]);
}

void Shader::SetParam(const char* name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(id_, name), x, y, z);
}

void Shader::SetParam(const char* name, const glm::vec4& value) const {
    glUniform4fv(glGetUniformLocation(id_, name), 1, &value[0]);
}

void Shader::SetParam(const char* name, float x, float y, float z,
                      float w) const {
    glUniform4f(glGetUniformLocation(id_, name), x, y, z, w);
}

void Shader::SetParam(const char* name, const glm::mat2& value) const {
    glUniformMatrix2fv(glGetUniformLocation(id_, name), 1, GL_FALSE,
                       &value[0][0]);
}

void Shader::SetParam(const char* name, const glm::mat3& value) const {
    glUniformMatrix3fv(glGetUniformLocation(id_, name), 1, GL_FALSE,
                       &value[0][0]);
}

void Shader::SetParam(const char* name, const glm::mat4& value) const {
    glUniformMatrix4fv(glGetUniformLocation(id_, name), 1, GL_FALSE,
                       &value[0][0]);
}

void Shader::SetParam(const char* name, const Color& color) const {
    SetParam(name, color.r, color.g, color.b, color.a);
}

void Shader::Bind() {
    assert(id_ != 0);
    glUseProgram(id_);
}

bool Shader::LogShaderErrorsIfAny(unsigned shader) {
    char message[1024];
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, nullptr, message);
        printf("Shader compilation error:\n");
        printf("%s\n", message);
    }
    return success != 0;
}

bool Shader::LogProgramErrorsIfAny(unsigned program) {
    char message[1024];
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 1024, nullptr, message);
        printf("Program compilation error:\n");
        printf("%s\n", message);
    }
    return success != 0;
}

void Cube::Create(float width, float depth, float height) {
    auto wOver2 = width / 2.f;
    auto hOver2 = height / 2.f;
    auto dOver2 = depth / 2.f;
    std::vector<glm::vec3> positions = {
        {-wOver2, -hOver2, -dOver2}, {-wOver2, hOver2, -dOver2},
        {wOver2, hOver2, -dOver2},   {-wOver2, -hOver2, -dOver2},
        {wOver2, hOver2, -dOver2},   {wOver2, -hOver2, -dOver2},
        {wOver2, -hOver2, -dOver2},  {wOver2, hOver2, -dOver2},
        {wOver2, -hOver2, dOver2},   {wOver2, -hOver2, dOver2},
        {wOver2, hOver2, -dOver2},   {wOver2, hOver2, dOver2},
        {wOver2, hOver2, -dOver2},   {-wOver2, hOver2, -dOver2},
        {-wOver2, hOver2, dOver2},   {wOver2, hOver2, -dOver2},
        {-wOver2, hOver2, dOver2},   {wOver2, hOver2, dOver2},
        {wOver2, -hOver2, dOver2},   {-wOver2, hOver2, dOver2},
        {-wOver2, -hOver2, dOver2},  {wOver2, -hOver2, dOver2},
        {wOver2, hOver2, dOver2},    {-wOver2, hOver2, dOver2},
        {-wOver2, -hOver2, dOver2},  {-wOver2, hOver2, -dOver2},
        {-wOver2, -hOver2, -dOver2}, {-wOver2, -hOver2, dOver2},
        {-wOver2, hOver2, dOver2},   {-wOver2, hOver2, -dOver2},
        {-wOver2, -hOver2, -dOver2}, {wOver2, -hOver2, -dOver2},
        {-wOver2, -hOver2, dOver2},  {wOver2, -hOver2, -dOver2},
        {wOver2, -hOver2, dOver2},   {-wOver2, -hOver2, dOver2},
    };

    std::vector<glm::vec3> normals;
    for (size_t i = 0; i < positions.size(); i += 3) {
        auto v1 = positions[i] - positions[i + 1];
        auto v2 = positions[i + 2] - positions[i + 1];
        auto normal = glm::normalize(glm::cross(v1, v2));
        normals.push_back(normal);
        normals.push_back(normal);
        normals.push_back(normal);
    }

    assert(positions.size() == normals.size());

    std::vector<glm::vec3> positions_normals;
    for (size_t i = 0; i < positions.size(); ++i) {
        positions_normals.push_back(positions[i]);
        positions_normals.push_back(normals[i]);
    }

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec3) * positions.size(),
                 positions_normals.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, 6 * sizeof(float),
                          (void*)(3 * sizeof(float)));
}

Cube::~Cube() {
    if (vbo_) {
        glDeleteBuffers(1, &vbo_);
    }
    if (vao_) {
        glDeleteVertexArrays(1, &vao_);
    }
}

void Cube::Render(Shader& shader, const Color& color, const glm::mat4& world,
                  const Camera& camera) {
    shader.Bind();
    shader.SetParam("world", world);
    shader.SetParam("view", camera.GetView());
    shader.SetParam("projection", camera.GetProjection());
    shader.SetParam("color", color);
    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Cube::RenderRefract(Shader& shader, const Color& color,
                         const SkyBox& skybox, const glm::mat4& world,
                         const Camera& camera) {
    shader.Bind();
    shader.SetParam("world", world);
    shader.SetParam("view", camera.GetView());
    shader.SetParam("projection", camera.GetProjection());
    shader.SetParam("camera_pos", camera.GetPosition());
    shader.SetParam("color", color);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.texture);
    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

BoardBounds::~BoardBounds() {
    if (vbo_ground_) {
        glDeleteBuffers(1, &vbo_ground_);
    }
    if (vao_ground_) {
        glDeleteVertexArrays(1, &vao_ground_);
    }
    if (vbo_wall_) {
        glDeleteBuffers(1, &vbo_wall_);
    }
    if (vao_wall_) {
        glDeleteVertexArrays(1, &vao_wall_);
    }
}

void BoardBounds::Create(const Board& board) {
    auto width = board.GetWidth();
    auto depth = board.GetDepth();
    auto height = board.GetHeight();
    board_dimensions_ = glm::ivec3(width, height, depth);
    // Ground creation
    {
        std::vector<glm::vec3> vertices;
        for (auto i = 0.f; i < width + 0.1f; i += 1.f) {
            vertices.emplace_back(i, 0.f, 0.f);
            vertices.emplace_back(i, 0.f, depth);
            vertices.emplace_back(0.f, 0.f, i);
            vertices.emplace_back(width, 0.f, i);
        }
        ground_vertex_count_ = vertices.size();

        glGenVertexArrays(1, &vao_ground_);
        glBindVertexArray(vao_ground_);

        glGenBuffers(1, &vbo_ground_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_ground_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(),
                     vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(glm::vec3),
                              (void*)0);
        glEnableVertexAttribArray(0);
    }

    // Wall creation
    {
        std::vector<glm::vec3> vertices;
        for (auto i = 0.f; i < width + 0.1f; i += 1.f) {
            vertices.emplace_back(i, 0.f, 0.f);
            vertices.emplace_back(i, height, 0.f);
        }

        for (auto i = 1.f; i < height + 0.1f; i += 1.f) {
            vertices.emplace_back(0.f, i, 0.f);
            vertices.emplace_back(width, i, 0.f);
        }

        wall_vertex_count_ = vertices.size();

        glGenVertexArrays(1, &vao_wall_);
        glBindVertexArray(vao_wall_);

        glGenBuffers(1, &vbo_wall_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_wall_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(),
                     vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(glm::vec3),
                              (void*)0);
        glEnableVertexAttribArray(0);
    }
}

void BoardBounds::Render(Shader& shader, const Color& color,
                         const Camera& camera) {
    // Render ground
    {
        shader.Bind();
        shader.SetParam("world", glm::mat4(1.f));
        shader.SetParam("view", camera.GetView());
        shader.SetParam("projection", camera.GetProjection());
        shader.SetParam("color", color);
        glBindVertexArray(vao_ground_);
        glDrawArrays(GL_LINES, 0, ground_vertex_count_);
    }

    {
        auto view_dir = camera.GetForward();
        {
            glm::vec3 wall_dir{0, 0, 1};
            if (glm::dot(view_dir, wall_dir) < 0.f) {
                glBindVertexArray(vao_wall_);
                glDrawArrays(GL_LINES, 0, wall_vertex_count_);
            }
        }

        {
            glm::vec3 wall_dir{1, 0, 0};
            if (glm::dot(view_dir, wall_dir) < 0.f) {
                glm::mat4 world(1.f);
                world = glm::rotate(world, -glm::pi<float>() / 2,
                                    glm::vec3(0.f, 1.f, 0.f));
                shader.SetParam("world", world);
                glBindVertexArray(vao_wall_);
                glDrawArrays(GL_LINES, 0, wall_vertex_count_);
            }
        }

        {
            glm::vec3 wall_dir{0, 0, -1};
            if (glm::dot(view_dir, wall_dir) < 0.f) {
                glm::mat4 world(1.f);
                world = glm::translate(
                    world, glm::vec3(0.f, 0.f, board_dimensions_.z));
                shader.SetParam("world", world);
                glBindVertexArray(vao_wall_);
                glDrawArrays(GL_LINES, 0, wall_vertex_count_);
            }
        }

        {
            glm::vec3 wall_dir{-1, 0, 0};
            if (glm::dot(view_dir, wall_dir) < 0.f) {
                glm::mat4 world(1.f);
                world = glm::rotate(world, -glm::pi<float>() / 2,
                                    glm::vec3(0.f, 1.f, 0.f));
                world = glm::translate(
                    world, glm::vec3(0.f, 0.f, -board_dimensions_.z));
                shader.SetParam("world", world);
                glBindVertexArray(vao_wall_);
                glDrawArrays(GL_LINES, 0, wall_vertex_count_);
            }
        }
    }
}

SkyBox::~SkyBox() {}

void SkyBox::Create() {
    float vertices[] = {
        -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f,
        1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,
        -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
        -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,
        -1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,
        -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f};

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), (void*)0);

    std::vector<std::string> paths = {
        "data/skybox/right.jpg", "data/skybox/left.jpg",
        "data/skybox/top.jpg",   "data/skybox/bottom.jpg",
        "data/skybox/front.jpg", "data/skybox/back.jpg",
    };
    texture = LoadCubemap(paths);
}

void SkyBox::Render(Shader& shader, const Camera& camera) {
    shader.Bind();
    shader.SetParam("skybox", 0);
    shader.SetParam("color", color);

    glDepthFunc(GL_LEQUAL);
    auto view_no_translation = glm::mat4(glm::mat3(camera.GetView()));
    shader.SetParam("view", view_no_translation);
    shader.SetParam("projection", camera.GetProjection());
    glBindVertexArray(vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}

unsigned SkyBox::LoadCubemap(const std::vector<std::string>& paths) {
    unsigned id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    int width, height, nr_components;
    for (unsigned int i = 0; i < paths.size(); i++) {
        unsigned char* data =
            stbi_load(paths[i].c_str(), &width, &height, &nr_components, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width,
                         height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            printf("Texuture load failure: %s\n", paths[i].c_str());
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return id;
}

AdvancedRenderer::AdvancedRenderer(Config& config) : config_(config) {}

void AdvancedRenderer::StartUp(const GameState& state) {
    solid_shader_.Create(solid_vs, solid_fs);
    solid_wire_shader_.Create(solid_wire_vs, solid_wire_fs);
    refract_shader_.Create(refract_vs, refract_fs);
    skybox_shader_.Create(skybox_vs, skybox_fs);
    tetris_cube_.Create(1.f, 1.f, 1.f);
    board_bounds_.Create(state.board);
    skybox_.Create();
}

void AdvancedRenderer::Render(const GameState& state,
                              const PerspectiveCamera& camera) {
    glViewport(0, 0, framebuffer_width_, framebuffer_height_);

    if (state.phase == GameState::Phase::Lost) {
        skybox_.color = {0.9f, 0.1f, 0.1f, 1.f};
    } else if (state.phase == GameState::Phase::LayersErase) {
        skybox_.color = {0.2f, 0.9f, 0.2f, 1.f};
    } else if (state.paused) {
        skybox_.color = {0.2f, 0.2f, 0.9f, 1.f};
    } else {
        skybox_.color = {1.f, 1.f, 1.f, 1.f};
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    if (config_.graphics_multisampling) {
        glEnable(GL_MULTISAMPLE);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    board_bounds_.Render(solid_wire_shader_, Color{0.3f, 0.3f, 0.3f}, camera);

    RenderBoard(state.board, camera);
    RenderFallingBlock(state.falling_block, camera);
    if (state.phase == GameState::Phase::BlockFalling) {
        RenderFallingBlockProjection(state, camera);
    }

    skybox_.Render(skybox_shader_, camera);

    // TODO(panmar): Make error checking more verbose
    {
        auto error = glGetError();
        if (error != GL_NO_ERROR) {
            printf("Error = %d\n", error);
        }
        assert(error == GL_NO_ERROR);
    }
}

void AdvancedRenderer::RenderBoard(const Board& board, const Camera& camera) {
    for (size_t k = 0; k < board.GetHeight(); ++k) {
        for (size_t i = 0; i < board.GetWidth(); ++i) {
            for (size_t j = 0; j < board.GetDepth(); ++j) {
                auto index = (i * board.GetWidth() + j) +
                             (k * board.GetWidth() * board.GetDepth());
                if (board.GetCell(index)) {
                    auto position = glm::vec3(i, k, j);
                    Color color = FromPackedColorToColor(board.GetCell(index));
                    RenderTetrisCube(position + glm::vec3(0.5f, 0.5f, 0.5f),
                                     color, camera);
                }
            }
        }
    }
}

void AdvancedRenderer::RenderFallingBlock(const Block& block,
                                          const Camera& camera) {
    Color color;
    color.r = block.GetColor().r / 255.f;
    color.g = block.GetColor().g / 255.f;
    color.b = block.GetColor().b / 255.f;

    for (auto& offset : block.GetCubeOffsets()) {
        auto position = glm::vec3(block.GetPosition() + offset) +
                        glm::vec3(0.5f, 0.5f, 0.5f);
        RenderTetrisCube(position, color, camera);
    }
}

void AdvancedRenderer::RenderFallingBlockProjection(const GameState& state,
                                                    const Camera& camera) {
    auto projected_block = state.falling_block;
    while (projected_block.IsValid(state.board)) {
        projected_block.Translate(glm::ivec3(0, -1, 0));
    }
    projected_block.Translate(glm::ivec3(0, 1, 0));

    Color color{0.9, 0.9f, 0.9f, 0.6f};

    glColorMask(false, false, false, false);
    for (auto& offset : projected_block.GetCubeOffsets()) {
        RenderCube(glm::vec3(projected_block.GetPosition() + offset) +
                       glm::vec3(0.5f, 0.5f, 0.5f),
                   color, glm::vec3(1.f, 1.f, 1.f), camera);
    }

    glColorMask(true, true, true, true);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (auto& offset : projected_block.GetCubeOffsets()) {
        RenderCube(glm::vec3(projected_block.GetPosition() + offset) +
                       glm::vec3(0.5f, 0.5f, 0.5f),
                   color, glm::vec3(1.f, 1.f, 1.f), camera);
    }
    glDisable(GL_BLEND);
    glDepthFunc(GL_LESS);
}

void AdvancedRenderer::RenderTetrisCube(const glm::vec3& pos,
                                        const Color& color,
                                        const Camera& camera) {
    Color cube_edge_color{0.1f, 0.1f, 0.1f};
    RenderCube(pos, cube_edge_color, glm::vec3(1.0f, 1.0f, 1.0f), camera);
    RenderRefractCube(pos, color, glm::vec3(1.01f, 0.8f, 0.8f), camera);
    RenderRefractCube(pos, color, glm::vec3(0.8f, 1.01f, 0.8f), camera);
    RenderRefractCube(pos, color, glm::vec3(0.8f, 0.8f, 1.01f), camera);
}

void AdvancedRenderer::RenderCube(const glm::vec3& position, const Color& color,
                                  const glm::vec3& scale,
                                  const Camera& camera) {

    glm::mat4 world(1.f);
    world = glm::translate(world, position);
    world = glm::scale(world, scale);
    tetris_cube_.Render(solid_shader_, color, world, camera);
}

void AdvancedRenderer::RenderRefractCube(const glm::vec3& position,
                                         const Color& color,
                                         const glm::vec3& scale,
                                         const Camera& camera) {
    glm::mat4 world(1.f);
    world = glm::translate(world, position);
    world = glm::scale(world, scale);
    tetris_cube_.RenderRefract(refract_shader_, color, skybox_, world, camera);
}

} // namespace tetris3d
