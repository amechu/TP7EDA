#pragma once

#include <string>

enum { ERROR1 = -1, ERROR2 = -2, ERROR3 = -3 };
const unsigned int NPARAMETERS = 1;
const unsigned int NKEYS = 1;
const unsigned int NVALUES = 1;

typedef struct
{
	char*  parameters[NPARAMETERS];
	char*  key[NKEYS];
	char*  value[NVALUES];

}parseData;

class Parser
{
public:
	Parser() { parseData.key[0] = (char*) "IP"; parseData.parameters[0] = (char*) "INICIO";  parseData.value[0] = (char*) "PLACEHOLDER"; };
	~Parser();
	int Read(int argc, char *argv[]);

	std::string myIP;

private:
	void Do(char *key, char *value);
	parseData parseData;
};

