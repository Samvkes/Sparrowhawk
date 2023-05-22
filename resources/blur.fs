#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform int shouldBlur;
uniform int screenWidth;
uniform int screenHeight;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables

void main()
{
	vec2 texSize = vec2(screenWidth,screenHeight);
	int size = 8;
	vec2 fragCoord = gl_FragCoord.xy;
    // Texel color fetching from texture sampler
	vec4 fcol = vec4(0);
	vec4 texelColor = texture(texture0, fragTexCoord);
	for (int i = -size; i<= size; ++i) {
		for (int j = -size; j<=size; ++j) {
		    fcol += texture(texture0, (gl_FragCoord.xy + vec2(i,j))/texSize);
		}
	}
    // Calculate final fragment color
	fcol /= ((size *2 + 1) * (size *2 + 1));
	if (shouldBlur == 1)
	{
		finalColor = fcol;
	}
	else 
	{
		finalColor = texelColor;
	}
	
}
