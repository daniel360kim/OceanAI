#include <iostream>

#include "parser.h"
#include "../include/json.hpp"

int main(int argc, char const *argv[])
{
    JsonParser parser;
    nlohmann::json json;
    parser.openJson("../data/data03_no_block.json", json);

    parser.json_to_csv(json, "../data/data03.csv");



    return 0;
}
