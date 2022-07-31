#!/usr/bin/env python3

import licant

#licant.include("imgui")
licant.include("crow")

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
		"nos",
		"crow"
	]
)

licant.cxx_application("viewer",
	sources=["viewer.cpp"],
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
	]
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
	]
)

licant.fileset("all", ["stream", "viewer", "camera"])

licant.ex("all")
