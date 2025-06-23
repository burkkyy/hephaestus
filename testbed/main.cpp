#include <hephaestus>
#include <iostream>
#include <stdexcept>

class TestBed : public hep::Application {
 public:
  TestBed(hep::ApplicationConfig config) : hep::Application(config) {}

  void onStart() {}

  void onRender(float deltaTime) {}

  void onClose() {}
};

int main(int argc, const char** argv) {
  (void)argc;
  (void)argv;
  std::cout << __FILE__ << "::" << __LINE__ << '\n';

  hep::ApplicationConfig config{
      .headless = false, .width = 750, .height = 1000, .name = "Hep"};

  hep::Hephaestus heph(std::make_unique<TestBed>(config));
  heph.launch();

  return EXIT_SUCCESS;
}
