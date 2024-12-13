/** 
 * @file engine/test/test.cpp
 * @author Caleb Burke
 * @date Nov 18, 2023
 */

#include <vulkan_plugin/window.hpp>
#include <vulkan_plugin/device.hpp>
#include <vulkan_plugin/swapchain.hpp>
#include <iostream>

static int TESTS = 0;
static int TESTS_PASSED = 0;

#define TEST_START(...)	TESTS++; std::cout << __VA_ARGS__ << "..... "
#define TEST_OK()	TESTS_PASSED++; std::cout << "\033[1;32m[OK]\033[0m" << '\n'
#define TEST_FAIL()	std::cout << "\033[1;31m[FAIL]\033[0m" << '\n'
#define MSG(...)	std::cout << "\033[1;34m[TEST]\033[0m " << __VA_ARGS__ << '\n'
#define TEST_ASSERT(b)	if(!(b)){ TEST_FAIL(); throw std::exception(); }

void RUN_TEST(const char* message, void (*func)()){
	TEST_START(message);
	try {
		(*func)();
		TEST_OK();
	} catch(std::exception& e){
		TEST_FAIL();
		std::cout << e.what() << std::endl;
	}
}

int main(int argc, const char** argv){
	(void)argc;
	(void)argv;
	
	MSG("Starting testing.");
	
	RUN_TEST("test", [](){
		int x = 0;
		x++;
	});

	RUN_TEST("Testing window", [](){
		hep::vul::Window window(100, 100, "testing");
	});
	
	RUN_TEST("Testing device", [](){
		hep::vul::Window window(100, 100, "testing");
		hep::vul::Device device(window);
	});

	RUN_TEST("Testing pipeline", [](){

	});
	
	RUN_TEST("Testing swapchain", [](){
        hep::vul::Window window(100, 100, "testing");
        hep::vul::Device device(window);
        hep::vul::Swapchain swapchain(device, window.get_extent());
	});

    MSG("Finished testing.");
	return 0;
}

