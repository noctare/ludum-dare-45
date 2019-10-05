#version 130

uniform mat4 model_view_projection;
uniform vec4 color;

in vec2 in_Position;
in vec4 in_Color;
in vec2 in_TexCoords;

out vec4 v_Color;
out vec2 v_TexCoords;

void main() {
	gl_Position = model_view_projection * vec4(in_Position.x, in_Position.y, 0.0f, 1.0f);
	v_Color = in_Color * color;
	v_TexCoords = in_TexCoords;
}
