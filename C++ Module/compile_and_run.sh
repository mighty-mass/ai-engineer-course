# Add JSON library
wget https://github.com/nlohmann/json/archive/refs/heads/develop.zip
unzip develop.zip "json-develop/include/nlohmann" -d "InsuraPro/nlohmann"

# Compile the program
#g++  -std=c++11 -IInsuraPro main.cpp -o main
g++  -std=c++20 -IInsuraPro main.cpp -o main

# Run the program
./main