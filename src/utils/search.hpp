#include <string>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

string initImeDialog(char *title, char *initial_text, int max_text_length);

json sortJson(string filter, json original);