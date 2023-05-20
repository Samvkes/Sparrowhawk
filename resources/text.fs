#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform float frameLoc;
uniform int oncog;
uniform int onminus;
uniform int oncross;
uniform int screenWidth;


// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables
vec3 colorA = vec3(0.9,0.3,0.912);
vec3 colorB = vec3(1.000,0.833,0.224);

void main()
{
	vec3 color = vec3(0.0);
	float pct = abs(sin(frameLoc));
	color = mix(colorA, colorB, pct);
	// float v = (sin(frameLoc) + 1.0) / 2;
	// float v1 = abs(sin(frameLoc*1.5));
	// float w1 = mix(0.7, 1.3, v1);
	vec2 fragCoord = gl_FragCoord.xy;
    // Texel color fetching from texture sampler
    vec4 texelColor = texture(texture0, fragTexCoord)*colDiffuse*fragColor;
	if (oncross == 1 && screenWidth - fragCoord.x < 50) 
	{
		finalColor = vec4(0.9, 0.2, 0.2, texelColor.a);
	}
	else if (onminus == 1 && screenWidth - fragCoord.x > 50 && screenWidth - fragCoord.x < 80) 
	{
		finalColor = vec4(0.4, 0.4, 1.0, texelColor.a);
	}
	else if (oncog == 1 && screenWidth - fragCoord.x > 80) 
	{
		finalColor = vec4(0.2, 0.9, 0.2, texelColor.a);
	}
	else
	{
    // Calculate final fragment color
		finalColor = vec4(color, texelColor.a);
	}
}
