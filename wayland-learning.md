# 强制 SDL 运行 Wayland
设置环境变量 **SDL_VIDEODRIVER=wayland**，然后运行可执行文件：

```bash
cd src
SDL_VIDEODRIVER=wayland ./sdl
```

如果 SDL 编译时启用了 Wayland 后端，这会强制使用 Wayland 显示。