#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// for printing out vectors
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

enum direction {
	BALL_UP,
	BALL_DOWN,
	BALL_LEFT,
	BALL_RIGHT
};

class Ball
{
public:
	glm::vec3 position;
	direction curr_dir;
	float segment_length; // the length of segment to detect collision
	float radius;

	Ball(glm::vec3 position, direction curr_dir,float radius)
	{
		this->position = position;
		this->curr_dir = curr_dir;
		this->segment_length = radius;
		this->radius = radius-0.07f;
	}

	////updates position
	//void move_forward()
	//{
	//	if (curr_dir == direction::BALL_UP)
	//	{
	//		position = glm::vec3(position.x, position.y, position.z + 0.01f);
	//	}
	//	else if(curr_dir == direction::BALL_DOWN)
	//	{
	//		position = glm::vec3(position.x, position.y, position.z - 0.01f);
	//	}
	//	else if (curr_dir == direction::BALL_LEFT)
	//	{
	//		position = glm::vec3(position.x + 0.01f, position.y, position.z);
	//	}
	//	else if (curr_dir == direction::BALL_RIGHT)
	//	{
	//		position = glm::vec3(position.x - 0.01f, position.y, position.z);
	//	}
	//}
	void rotate_right()
	{
		if (curr_dir == direction::BALL_UP)
		{
			curr_dir = direction::BALL_RIGHT;
		}
		else if (curr_dir == direction::BALL_RIGHT)
		{
			curr_dir = direction::BALL_DOWN;
		}
		else if (curr_dir == direction::BALL_DOWN)
		{
			curr_dir = direction::BALL_LEFT;
		}
		else if (curr_dir == direction::BALL_LEFT)
		{
			curr_dir = direction::BALL_UP;
		}
	}
	//update position
	glm::mat4 moveforward(glm::mat4 curr_model,float custom_move = 0.01f)
	{
		if (curr_dir == direction::BALL_UP)
		{
			position = glm::vec3(position.x, position.y, position.z - custom_move);
			glm::mat4 new_model = glm::translate(curr_model, position);
			return glm::rotate(new_model, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
		}
		else if (curr_dir == direction::BALL_RIGHT)
		{
			position = glm::vec3(position.x + custom_move, position.y, position.z);
			glm::mat4 new_model = glm::translate(curr_model, position);
			return glm::rotate(new_model, glm::radians(0.0f), glm::vec3(0.0, 1.0, 0.0));
		}
		else if (curr_dir == direction::BALL_DOWN)
		{
			position = glm::vec3(position.x, position.y, position.z + custom_move);
			glm::mat4 new_model = glm::translate(curr_model, position);
			return glm::rotate(new_model, glm::radians(270.0f), glm::vec3(0.0, 1.0, 0.0));
		}
		else if (curr_dir == direction::BALL_LEFT)
		{
			position = glm::vec3(position.x - custom_move, position.y, position.z);
			glm::mat4 new_model = glm::translate(curr_model, position);
			return glm::rotate(new_model, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
		}
		return curr_model;
	}
	// a line from top to the right to calculate clashing
	glm::vec3 top_right_endpoint()
	{
		if (curr_dir == direction::BALL_UP)
		{
			return glm::vec3(position.x + segment_length, position.y, position.z - radius);
		}
		else if (curr_dir == direction::BALL_RIGHT)
		{
			return glm::vec3(position.x + radius, position.y, position.z + segment_length);
		}
		else if (curr_dir == direction::BALL_DOWN)
		{
			return glm::vec3(position.x - segment_length, position.y, position.z + radius);
		}
		else if (curr_dir == direction::BALL_LEFT)
		{
			return glm::vec3(position.x - radius, position.y, position.z - segment_length);
		}
		std::cout << "Error" << std::endl;
		return position;
	}
	//a line from center to the right to calculate clashing
	glm::vec3 center_right_endpoint()
	{
		if (curr_dir == direction::BALL_UP)
		{
			return glm::vec3(position.x + segment_length, position.y, position.z);
		}
		else if (curr_dir == direction::BALL_RIGHT)
		{
			return glm::vec3(position.x, position.y, position.z + segment_length);
		}
		else if (curr_dir == direction::BALL_DOWN)
		{
			return glm::vec3(position.x - segment_length, position.y, position.z);
		}
		else if (curr_dir == direction::BALL_LEFT)
		{
			return glm::vec3(position.x, position.y, position.z - segment_length);
		}
		std::cout << "Error" << std::endl;
		return position;
	}

	// a line from bottom to the right to calculate clashing
	glm::vec3 bottom_right_endpoint()
	{
		if (curr_dir == direction::BALL_UP)
		{
			return glm::vec3(position.x + segment_length, position.y, position.z + radius);
		}
		else if (curr_dir == direction::BALL_RIGHT)
		{
			return glm::vec3(position.x - radius, position.y, position.z + segment_length);
		}
		else if (curr_dir == direction::BALL_DOWN)
		{
			return glm::vec3(position.x - segment_length, position.y, position.z - radius);
		}
		else if (curr_dir == direction::BALL_LEFT)
		{
			return glm::vec3(position.x + radius, position.y, position.z - segment_length);
		}
		std::cout << "Error" << std::endl;
		return position;
	}

	// a line from top to the left to calculate clashing
	glm::vec3 top_left_endpoint()
	{
		if (curr_dir == direction::BALL_UP)
		{
			return glm::vec3(position.x - segment_length, position.y, position.z - radius);
		}
		else if (curr_dir == direction::BALL_RIGHT)
		{
			return glm::vec3(position.x + radius, position.y, position.z - segment_length);
		}
		else if (curr_dir == direction::BALL_DOWN)
		{
			return glm::vec3(position.x + segment_length, position.y, position.z + radius);
		}
		else if (curr_dir == direction::BALL_LEFT)
		{
			return glm::vec3(position.x - radius, position.y, position.z + segment_length);
		}
		std::cout << "Error" << std::endl;
		return position;
	}
	//a line from center to the left to calculate clashing
	glm::vec3 center_left_endpoint()
	{
		if (curr_dir == direction::BALL_UP)
		{
			return glm::vec3(position.x - segment_length, position.y, position.z);
		}
		else if (curr_dir == direction::BALL_RIGHT)
		{
			return glm::vec3(position.x, position.y, position.z - segment_length);
		}
		else if (curr_dir == direction::BALL_DOWN)
		{
			return glm::vec3(position.x + segment_length, position.y, position.z);
		}
		else if (curr_dir == direction::BALL_LEFT)
		{
			return glm::vec3(position.x, position.y, position.z + segment_length);
		}
		std::cout << "Error" << std::endl;
		return position;
	}

	// a line from bottom to the left to calculate clashing
	glm::vec3 bottom_left_endpoint()
	{
		if (curr_dir == direction::BALL_UP)
		{
			return glm::vec3(position.x - segment_length, position.y, position.z + radius);
		}
		else if (curr_dir == direction::BALL_RIGHT)
		{
			return glm::vec3(position.x - radius, position.y, position.z - segment_length);
		}
		else if (curr_dir == direction::BALL_DOWN)
		{
			return glm::vec3(position.x + segment_length, position.y, position.z - radius);
		}
		else if (curr_dir == direction::BALL_LEFT)
		{
			return glm::vec3(position.x + radius, position.y, position.z + segment_length);
		}
		std::cout << "Error" << std::endl;
		return position;
	}
	//a line to the front to calculate clashing
	glm::vec3 forward_endpoint()
	{
		
		if (curr_dir == direction::BALL_UP)
		{
			return glm::vec3(position.x, position.y, position.z - segment_length);
		}
		else if (curr_dir == direction::BALL_RIGHT)
		{
			return glm::vec3(position.x + segment_length, position.y, position.z);
		}
		else if (curr_dir == direction::BALL_DOWN)
		{
			return glm::vec3(position.x, position.y, position.z + segment_length);
		}
		else if (curr_dir == direction::BALL_LEFT)
		{
			return glm::vec3(position.x - segment_length, position.y, position.z);
		}
		std::cout << "Error" << std::endl;
		return position;
	}

};
