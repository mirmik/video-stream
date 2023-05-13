#!/usr/bin/env python3

import licant

licant.cxx_application("stream",
	sources=["stream.cpp"],
	include_paths=["/usr/include/opencv4"],
	libs=["GL", "GLEW", "glfw", 
		"opencv_core",
		"opencv_highgui",
		"opencv_imgcodecs",
		"opencv_imgproc",
		"opencv_videoio",

#		"openh264",
		
		"pthread",
		"igris",
		"rabbit",
		"nos",
		"crow"
	],
	cxx_flags="-fPIC -Wextra",
	cc_flags="-fPIC",
)

#licant.cxx_application("viewer",
#	sources=["viewer.cpp",
#		"imgui/backends/imgui_impl_glfw.cpp",
#		"imgui/backends/imgui_impl_opengl3.cpp",
#		"imgui/imgui.cpp",
#		"imgui/imgui_tables.cpp",
#		"imgui/imgui_demo.cpp",
#		"imgui/imgui_widgets.cpp",
#		"imgui/imgui_draw.cpp",
#	],
#	include_paths=["/usr/include/opencv4", "./imgui", "./imgui/backends"],
#	libs=["GL", "GLEW", "glfw", "GLU", "stb",
#		"opencv_core",
#		"opencv_highgui",
##		"opencv_imgcodecs",
#		"opencv_videoio",
#		
#		"pthread",
#		"igris",
#		"rabbit",
#		"nos",
#		"crow"
#	],
#	cxx_flags="-fPIC -Wextra",
#	cc_flags="-fPIC",
#)

licant.cxx_application("glviewer",
	sources=["glviewer.cpp"
	],
	include_paths=["/usr/include/opencv4"],
	libs=["GL", "GLEW", "glfw", "GLU", "stb",
		"opencv_core",
		"opencv_highgui",
		"opencv_imgcodecs",
		"opencv_videoio",
		
		"pthread",
		"igris",
		"rabbit",
		"nos",
		"crow"
	],
	cxx_flags="-fPIC -Wextra",
	cc_flags="-fPIC",
)

licant.cxx_application("synced-viewer",
	sources=["synced-viewer.cpp"
	],
	include_paths=["/usr/include/opencv4"],
	libs=["GL", "GLEW", "glfw", "GLU", "stb",
		"opencv_core",
		"opencv_highgui",
		"opencv_imgcodecs",
		"opencv_videoio",
		
		"pthread",
		"igris",
		"rabbit",
		"nos",
		"crow"
	],
	cxx_flags="-fPIC -Wextra",
	cc_flags="-fPIC",
)

licant.cxx_application("imgui-viewer",
	sources=["imgui-viewer.cpp",
		"imgui/backends/imgui_impl_glfw.cpp",
		"imgui/backends/imgui_impl_opengl3.cpp",
		"imgui/imgui.cpp",
		"imgui/imgui_tables.cpp",
		"imgui/imgui_demo.cpp",
		"imgui/imgui_widgets.cpp",
		"imgui/imgui_draw.cpp",
	],
	include_paths=["/usr/include/opencv4", "./imgui", "./imgui/backends"],
	libs=["GL", "GLEW", "glfw", "GLU", "stb",
		"opencv_core",
		"opencv_highgui",
		"opencv_imgcodecs",
		"opencv_videoio",
		
		"pthread",
		"igris",
		"rabbit",
		"nos",
		"crow"
	],
	cxx_flags="-fPIC -Wextra -g",
	cc_flags="-fPIC -g",
)


licant.cxx_application("camera",
	sources=["camera.cpp"],
	include_paths=["/usr/include/opencv4"],
	libs=["GL", "GLEW", "glfw", 
		"opencv_core",
		"opencv_highgui",
		"opencv_imgcodecs",
		"opencv_videoio",
		
		"pthread",
		"igris",
		"nos",
		"crow"
	],
	cxx_flags="-fPIC -Wextra -g",
	cc_flags="-fPIC -g",
)

licant.fileset("all", ["stream", "camera", "glviewer", "synced-viewer"])

licant.ex("all")
