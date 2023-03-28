echo "building shaders!"
rm vert.spv frag.spv vert_ui.spv frag_ui.spv
glslc shader.vert -o vert.spv
glslc shader.frag -o frag.spv
glslc shader_ui.vert -o vert_ui.spv
glslc shader_ui.frag -o frag_ui.spv

echo "copying shader...."
mkdir -p ../../../build/vkui/shaders
cp *.spv ../../../build/vkui/shaders/

