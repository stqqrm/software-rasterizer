#include "app.h"

int main() {
	core::core::Initialize();
	core::math_init();
	core::asset_manager::Init();
	app a;
	int r = a.Run(APP_NAME, APP_WIDTH, APP_HEIGHT);
	core::core::UnInitialize();
	return r;
}
