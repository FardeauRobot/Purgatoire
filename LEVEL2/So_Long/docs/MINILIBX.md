# MiniLibX Cheat Sheet

A concise map of the core MiniLibX (Linux) functions, their roles, and how they work together. This favors practical ordering and lifecycle flow so you can slot code into `so_long` quickly.

## Init and Window
- `mlx_init()` → returns MLX context handle. Call once; if NULL, bail.
- `mlx_new_window(mlx, width, height, title)` → create window. Returns window handle or NULL.
- `mlx_clear_window(mlx, win)` → clear window to background color.
- `mlx_destroy_window(mlx, win)` → close window and free related resources.

## Images and Pixels
- `mlx_new_image(mlx, width, height)` → create an off-screen image buffer. Returns image handle or NULL.
- `mlx_get_data_addr(img, &bpp, &line_len, &endian)` → map the image buffer for pixel writes; gives you raw pointer and layout info.
- `mlx_put_image_to_window(mlx, win, img, x, y)` → blit an image onto the window at position `(x, y)`.
- `mlx_destroy_image(mlx, img)` → free image resources.

## XPM Assets
- `mlx_xpm_file_to_image(mlx, path, &w, &h)` → load an XPM file into an image. Returns image handle or NULL.
- `mlx_xpm_to_image` (alias in some builds) works similarly.

## Color Helpers
- `mlx_get_color_value(mlx, rgb)` → convert an RGB int to a display-appropriate value.
- `mlx_rgb_to_color(int r, int g, int b)` (some dists) → compose a color; otherwise shift `(r << 16 | g << 8 | b)` manually.

## Event Loop and Hooks
- `mlx_loop(mlx)` → enter the main event loop. Must be the last call after hooks are set.
- `mlx_loop_hook(mlx, func, param)` → per-frame callback; good for animations or redraws without input.
- `mlx_key_hook(win, func, param)` → keyboard events (simple key press). Signature typically `int func(int keycode, void *param)`.
- `mlx_mouse_hook(win, func, param)` → mouse button events. Signature `int func(int button, int x, int y, void *param)`.
- `mlx_hook(win, event, mask, func, param)` → low-level hook for specific X11 events (e.g., destroy notify). Signature varies by event; common to use for window close (`event 17`, mask `1L << 17`).

## Common Event Codes (Linux build)
- Close window: event `17`, mask `0` or `1L<<17` depending on header version. Use `mlx_hook(win, 17, 0, on_destroy, param)`.
- Key press: event `2`, mask `1L<<0` (often handled by `mlx_key_hook`).

## Typical Lifecycle
1) `mlx = mlx_init();` → check NULL.
2) `win = mlx_new_window(mlx, w, h, "so_long");` → check NULL.
3) Load assets: `img = mlx_xpm_file_to_image(mlx, path, &iw, &ih);` per tile.
4) For dynamic pixels, create image and map buffer: `img = mlx_new_image`; then `addr = mlx_get_data_addr(img, &bpp, &line, &endian);` write pixels; `mlx_put_image_to_window` to draw.
5) Install hooks:
   - `mlx_key_hook(win, on_key, data);` for key inputs.
   - `mlx_hook(win, 17, 0, on_destroy, data);` to handle window close.
   - Optionally `mlx_loop_hook(mlx, on_frame, data);` for per-frame updates.
6) `mlx_loop(mlx);` → blocks until exit.
7) Cleanup on exit:
   - Destroy images: `mlx_destroy_image` for every loaded/created image.
   - Destroy window: `mlx_destroy_window`.
   - If your distro provides `mlx_destroy_display(mlx)`, call it then `free(mlx);` (Linux version often needs this to avoid X resource leaks).

## Error-Handling Pointers
- Always check returns of `mlx_init`, `mlx_new_window`, and image loaders.
- On any failure after partial allocation, destroy what you already created to avoid leaks.
- Keep all MLX handles (mlx, win, images) in a single struct so hooks can access and free them.

## Minimal Skeleton (C)
```c
#include "mlx.h"

typedef struct s_app {
    void *mlx;
    void *win;
    void *img;
}   t_app;

int on_destroy(t_app *app)
{
    if (app->img)
        mlx_destroy_image(app->mlx, app->img);
    if (app->win)
        mlx_destroy_window(app->mlx, app->win);
#ifdef linux
    mlx_destroy_display(app->mlx);
#endif
    free(app->mlx);
    exit(0);
}

int on_key(int keycode, t_app *app)
{
    if (keycode == 65307) // ESC
        on_destroy(app);
    return (0);
}

int main(void)
{
    t_app app = {0};

    app.mlx = mlx_init();
    if (!app.mlx)
        return (1);
    app.win = mlx_new_window(app.mlx, 640, 480, "so_long");
    if (!app.win)
        return (1);
    // load or create images here
    mlx_key_hook(app.win, on_key, &app);
    mlx_hook(app.win, 17, 0, on_destroy, &app); // window close
    mlx_loop(app.mlx);
    return (0);
}
```
