cmake_minimum_required(VERSION 3.22)
project(MACAttack)


add_executable(MACAttack macAttack.cpp)

target_include_directories(MACAttack PUBLIC
                            "../Include/boost_1_62_0/"
                            )
# target_link_libraries(MACAttack INTERFACE boost_dynamic_bitset)
