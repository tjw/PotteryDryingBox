
class Temperature {
public:
	static void setup(void);
	
	typedef struct {
		float temperature;
		float humidity;
	} Result;
	static bool read(Result &result);
};
