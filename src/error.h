#ifndef TETRIS3D_ERROR_H_
#define TETRIS3D_ERROR_H_

#include <stdexcept>
#include <string>

namespace tetris3d {

class Error : public std::runtime_error {
public:
    explicit Error(std::string message)
        : std::runtime_error(std::move(message)) {}
};

} // namespace tetris3d

#endif // TETRIS3D_ERROR_H_