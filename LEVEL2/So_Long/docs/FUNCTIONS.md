# MiniLibX Functions — Detailed Notes

Function-by-function breakdown for the Linux MiniLibX used in `so_long`.

## Index
- [Init and Window](#init-and-window)
- [Images and Pixels](#images-and-pixels)
- [XPM Assets](#xpm-assets)
- [Colors](#colors)
- [Event Loop and Hooks](#event-loop-and-hooks)
- [Cleanup Sequence](#cleanup-sequence)
- [Typical Failure Points](#typical-failure-points)

## Init and Window
- **mlx_init**
  - Purpose: open a connection to the X server and create the MLX context.
  - Signature: `void *mlx_init(void);`
  - Returns: MLX handle or `NULL` if the display cannot be opened (e.g., missing X11, SSH without forwarding).
  - Usage: call once at startup; required before any other MLX calls.
- **mlx_new_window(mlx, width, height, title)**
  - Purpose: create a drawable window bound to the MLX context.
  - Signature: `void *mlx_new_window(void *mlx, int w, int h, char *title);`
  - Returns: window handle or `NULL` on failure.
  - Notes: must be destroyed before tearing down the MLX context; size should fit your map.
- **mlx_clear_window(mlx, win)**
  - Purpose: clear the window to its background color.
  - Use when you want to redraw the entire frame without residual pixels.
- **mlx_destroy_window(mlx, win)**
  - Purpose: close a window and free associated X11 resources.
  - Call before destroying the display to avoid leaks.
- **mlx_destroy_display(mlx)** (Linux build)
  - Purpose: close the X11 display connection for the MLX context.
  - Call after all windows/images are destroyed, then `free(mlx);`.

## Images and Pixels
- **mlx_new_image(mlx, width, height)**
  - Purpose: create an off-screen image buffer for drawing.
  - Returns: image handle or `NULL` if allocation fails.
- **mlx_get_data_addr(img, &bpp, &line_len, &endian)**
  - Purpose: access raw pixel buffer for an image.
  - Outputs:
    - `bpp`: bits per pixel (commonly 32)
    - `line_len`: bytes per row (stride)
    - `endian`: 0 for little-endian on x86
  - Returns: pointer to pixel data; write pixels then blit with `mlx_put_image_to_window`.
- **mlx_put_image_to_window(mlx, win, img, x, y)**
  - Purpose: draw an image to the window at `(x, y)` (top-left anchor).
- **mlx_destroy_image(mlx, img)**
  - Purpose: free an image created via `mlx_new_image` or XPM loader.
  - Required for every image to avoid leaks.

## XPM Assets
- **mlx_xpm_file_to_image(mlx, path, &w, &h)**
  - Purpose: load an XPM sprite into an MLX image.
  - Returns: image handle or `NULL` if the path is wrong or the file is invalid.
  - Outputs: fills `w` and `h` with the image dimensions (useful for tile size checks).

## Colors
- **mlx_get_color_value(mlx, rgb)**
  - Purpose: convert an RGB integer to the display’s pixel value (handles visual specifics).
- **mlx_rgb_to_color(r, g, b)** (if provided by your distro)
  - Purpose: compose a color; otherwise, use `(r << 16) | (g << 8) | b`.

## Event Loop and Hooks
- **mlx_loop(mlx)**
  - Purpose: start the main event loop; blocks until exit.
  - Set all hooks before calling.
- **mlx_loop_hook(mlx, func, param)**
  - Purpose: per-iteration callback (use for redraw/animation/game tick).
  - Signature: `int func(void *param);`
- **mlx_key_hook(win, func, param)**
  - Purpose: basic key press handling.
  - Signature: `int func(int keycode, void *param);`
- **mlx_mouse_hook(win, func, param)**
  - Purpose: mouse button events.
  - Signature: `int func(int button, int x, int y, void *param);`
- **mlx_hook(win, event, mask, func, param)**
  - Purpose: low-level hook for specific X11 events.
  - Common: window close = event `17`, mask `0` or `1L << 17` depending on header; use to handle graceful exit.

## Cleanup Sequence
1) Destroy every image with `mlx_destroy_image`.
2) Destroy the window with `mlx_destroy_window`.
3) (Linux) call `mlx_destroy_display(mlx);`
4) `free(mlx);`

## Typical Failure Points
- `mlx_init` returns `NULL` when X11 is unavailable (no display/forwarding).
- `mlx_xpm_file_to_image` returns `NULL` on bad path or malformed XPM.
- Forgetting to destroy images/windows leaks X resources; skipping `mlx_destroy_display` (Linux) can leave the display connection open.
