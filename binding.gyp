{
    'target_defaults': {
		"conditions": [
			[
				"OS == 'mac'", {
					"include_dirs": [".", "..", "osx", "osx/include"],
					"libraries": [
						"-framework Carbon",
						"-framework Cocoa",
						"-framework OpenGL",
						"-framework IOKit"
					]
				}
			],
					
			[
				"OS == 'win'", {
					"include_dirs": [".", "..", "windows", "windows/include"],
					"libraries": [
						"-lwinmm",
						"-ldwmapi",
						"-lopengl32",
						"-lgdi32",
						"../windows/SDL2.lib"
					]
				}
			]
		]
    },

	"targets": [
		{
			"target_name": "nodeGamepad",
			"sources": ["node-gamepad.cpp"],
		}
	]
}
