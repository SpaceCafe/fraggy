cmake_minimum_required(VERSION 3.17)
project(fraggy)
add_executable(fraggy.agg fraggy.agg.c fraggy.agg.h distribution.c distribution.h)
configure_file(fraggy.anlz fraggy.anlz COPYONLY)
