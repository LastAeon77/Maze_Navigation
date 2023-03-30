# Maze_Navigation
Maze Navigation with OpenGL. Navigate by detecting clashes.

Most credits for shader.h, mesh.h, model.h, camera.h and basic configurations goes to https://learnopengl.com/

I also used many external libraries downloaded through vcpkg. https://vcpkg.io/en/

mainly: glad (glfw version check), glfw 3.3, glm (OpenGL math)

Main modifications I made are to model.h and mesh.h, where I added clashing logic by seeing if a line passes through any of the triangles. 

Through this clash logic, I create states that the ball reacts to upon clashing in main.cpp:

		// state 1: there is space on the right. Turn right.
		// state 2: There is space only on the left. Turn right 3 times (turn left)
		// state 3: there is no space in the front. There is no space on the left. There is no space on the right. Turn back.
		// default: no space on right but space in front: forward
    
    
