#ifndef INPUT_HPP
#define INPUT_HPP

#include <ctrl.h>

namespace vmp { namespace input {

	struct AnalogData {
		SceUInt8 lx;
		SceUInt8 ly;
		SceUInt8 rx;
		SceUInt8 ry;	
    };

	class Input
	{
		private:
			SceCtrlData _old, _new;
			AnalogData _analog;

			Input (void);
			~Input (void);

		public:
			static Input *getInstance(void);
			void poll(void);
			AnalogData getAnalog(void);
			SceBool pressed(SceUInt32 buttons);
			SceBool released(SceUInt32 buttons);
			SceBool held(SceUInt32 buttons);
	};

}};

#endif