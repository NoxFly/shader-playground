/**
 * @author Dorian Thivolle
 */

#include "App.hpp"
#include "utils.hpp"


int main(int argc, char** argv)
{
	std::string ipt = "";

	App app;

	std::cout << "====== Welcome to Shader Playground ! ======\n"
		<< "Author : Noxfly\n\n"
		<< "Type \"quit\" or \"exit\" to quit.\n"
		<< "  1. Write fragment shaders (.frag) in res/shaders/ folder.\n"
		<< "  2. Run this application\n"
		<< "  3. Enter a .frag file name (without path).\n\n"
		<< "Here some keys :\n"
		<< "- \"Esc\" when the window is opened to quit.\n"
		<< "- \"F5\" to hot-reload the current shader.\n"
		<< "- \"F8\" to toggle FPS limit.\n"
		<< "- \"F9\" to reset variables (zoom, camera, ...).\n"
		<< "- \"F11\" to toggle fullscreen borderless (keep the OS taskbar).\n\n"
		<< "For your fragment shaders, it will be included in the main fragment shader code.\n"
		<< "So, you don't have to write the main function, neither declare the in/out/uniform variables and the #version.\n\n"
		<< "You can use the #include directive to include other files, as follow : #include <path/nameWithoutExt>.\n"
		<< "It will search for a .glsl file in the res/shaders/ folder.\n"
		<< "\nHave fun !\n\n"
		<< std::endl;

	while (ipt != "quit" && ipt != "exit") {
		std::cout << "> ";
		std::cin >> ipt;

		ipt = trim(ipt);

		if (ipt != "quit" && ipt != "exit") {
			if (app.loadFractal(ipt)) {
				app.run();
			}
			else {
				std::cout << "Fractal not found.\n" << std::endl;
			}
		}
	}

	return EXIT_SUCCESS;
}