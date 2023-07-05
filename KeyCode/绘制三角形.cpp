#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// 绘制三角形
// 顶点数据
const float triangle[] = {
	-0.5f, -0.5f, 0.0f,	// 左下
	 0.5f, -0.5f, 0.0f,	// 右下
	 0.0f,  0.5f, 0.0f,	// 正上
};

int screen_width = 1280;
int screen_height = 720;

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_ANY_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// 使用核心模式，无需向后兼容
	glfwWindowHint(GLFW_RESIZABLE, false);	// 不可改变窗口大小

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(screen_width, screen_height, "Quadrilateral", NULL, NULL);
	if (!window)
	{
		std::cout << "Failed to Create OpenGL Context" << std::endl;
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);	// 将窗口的上下文设置为当前线程的主上下文

	// 初始化GLAD，加载OpenGL函数指针地址的函数
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	// 指定当前视口尺寸（前两个参数为左下角位置，后两个参数为渲染窗口的宽和高）
	glViewport(0, 0, screen_width, screen_height);
	
	// 将顶点数据存储到显存
	// 生成三角形的VAO、VBO
	GLuint vertex_array_object;		// VAO
	glGenVertexArrays(1, &vertex_array_object);
	glBindVertexArray(vertex_array_object);

	GLuint vertex_buffer_object;	// VBO
	glGenBuffers(1, &vertex_buffer_object);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
	// 将顶点数据绑定至当前默认的缓冲中
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);	// static draw表示顶点数据不会被改变
	
	// 设置顶点属性指针，告诉GPU我们要如何处理显存中的数据
	// 参数1：表示顶点着色器位置值
	// 参数2：表示是几分量的向量
	// 参数3：表示顶点类型
	// 参数4：表示是否希望数据被标准化（即映射到0~1之间）
	// 参数5：步长，表示连续顶点属性间的间隔，表示下一组数据在三个float变量之后
	// 参数6：数据偏移量，由于位置属性在数组开头，所以偏移为0，且由于参数类型限制，必须将其进行强制类型转换
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);	// 表示开启0通道
	// 解绑VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// 注意不要解绑EBO，因为EBO存储在VAO中
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	// 解绑VAO
	glBindVertexArray(0);

	// 顶点着色器和片段着色器源码
	// 将顶点位置作为顶点着色器的输出
	const char* vertex_shader_source =
		"#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"	// 位置变量的属性位置值为0
		"void main()\n"
		"{\n"
		"	gl_Position = vec4(aPos, 1.0);\n"
		"}\n\0";
	const char* fragment_shader_source =
		"#version 330 core\n"
		"out vec4 FragColor;\n"	// 输出的颜色向量
		"void main()\n"
		"{\n"
		"	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"// 三角形的颜色
		"}\n\0";

	// 生成并编译着色器
	// 顶点着色器
	int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
	glCompileShader(vertex_shader);
	int success;
	char info_log[512];
	// 检查着色器是否编译成功，如果编译失败，打印错误信息
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex_shader, 512, nullptr, info_log);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << std::endl;
	}

	// 片段着色器
	int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
	glCompileShader(fragment_shader);
	// 检查着色器是否成功编译，如果编译失败，打印错误信息
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment_shader, 512, nullptr, info_log);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << info_log << std::endl;
	}
	
	// 链接顶点和片段着色器至一个着色器程序
	int shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);
	// 检查着色器是否链接成功，如果链接失败，打印错误信息
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shader_program, 512, nullptr, info_log);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << std::endl;
	}

	// 删除着色器
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	// 线框模式
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		// 清空颜色缓冲
		glClearColor(0.0f, 0.34f, 0.57f, 1.0f);	// 背景色
		glClear(GL_COLOR_BUFFER_BIT);

		// 使用着色器程序
		glUseProgram(shader_program);

		// 绘制三角形
		glBindVertexArray(vertex_array_object);
		glDrawArrays(GL_TRIANGLES, 0, 3);	// 第二个参数表示顶点数组的起始索引值，第三个参数表示绘制的顶点数量
		glBindVertexArray(0);	// 解除绑定

		// 双缓冲，渲染指令都在离屏缓冲（后缓冲）中执行，执行完毕后交换前后缓冲
		// 交换缓冲
		glfwSwapBuffers(window);
		// 检查是否有触发事件（比如键盘输入、鼠标移动等）
		glfwPollEvents();
	}

	// 删除VAO和VBO
	glDeleteVertexArrays(1, &vertex_array_object);
	glDeleteBuffers(1, &vertex_buffer_object);

	// 清理所有的资源并正确退出程序
	glfwTerminate();
	return 0;
}
