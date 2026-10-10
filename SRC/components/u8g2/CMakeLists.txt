# u8g2 (https://github.com/olikraus/u8g2) - solo la API en C (csrc)
file(GLOB U8G2_SRCS csrc/*.c)
idf_component_register(SRCS ${U8G2_SRCS} INCLUDE_DIRS csrc)
target_compile_options(${COMPONENT_LIB} PRIVATE -w)
