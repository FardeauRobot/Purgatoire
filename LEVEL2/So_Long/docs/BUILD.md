# Build

## Prerequisites
- Linux with X11 development headers (`xorg`, `x11`, `Xext`), and `make`, `cc` (gcc/clang)
- The bundled libraries are built locally:
  - `includes/libft`
  - `includes/minilibx-linux`

## Commands
- Build everything: `make`
- Clean objects: `make clean`
- Clean objects and binary: `make fclean`
- Rebuild from scratch: `make re`

Artifacts:
- Binary: `so_long`
- Objects: `objs/`
- Libs: `includes/libft/libft.a`, `includes/minilibx-linux/libmlx.a`
