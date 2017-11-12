#include <crunch++.h>
#include <conversions.hxx>
#include <testConversions.hxx>

template<typename int_t> struct testFromInt_t
{
private:
	using fromInt = fromInt_t<int_t, int_t>;

public:
	void testConversions(testsuit &suit, const testOk_t<int_t> &okTests, const testFailInt_t<int_t> &failTests)
	{
		for (const auto &test : okTests)
		{
			fromInt conv(test.first);
		}

		for (const auto &test : failTests)
		{
			fromInt conv(test);
		}
	}
};

void testUint8(testsuit &suit, const testOk_t<uint8_t> &okTests, const testFailInt_t<uint8_t> &failTests)
	{ testFromInt_t<uint8_t> tests; tests.testConversions(suit, okTests, failTests); }
void testInt8(testsuit &suit, const testOk_t<int8_t> &okTests, const testFailInt_t<int8_t> &failTests)
	{ testFromInt_t<int8_t> tests; tests.testConversions(suit, okTests, failTests); }
