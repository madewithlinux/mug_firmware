import svelte from 'rollup-plugin-svelte';
import commonjs from '@rollup/plugin-commonjs';
import resolve from '@rollup/plugin-node-resolve';
import livereload from 'rollup-plugin-livereload';
import { terser } from 'rollup-plugin-terser';
import sveltePreprocess from 'svelte-preprocess';
import typescript from '@rollup/plugin-typescript';
import { visualizer } from 'rollup-plugin-visualizer';
import css from 'rollup-plugin-css-only';
import html from '@rollup/plugin-html';

const production = !process.env.ROLLUP_WATCH;

function serve() {
	let server;

	function toExit() {
		if (server) server.kill(0);
	}

	return {
		writeBundle() {
			if (server) return;
			server = require('child_process').spawn('npm', ['run', 'start', '--', '--dev'], {
				stdio: ['ignore', 'inherit', 'inherit'],
				shell: true
			});

			process.on('SIGTERM', toExit);
			process.on('exit', toExit);
		}
	};
}

export default {
	input: 'src/main.ts',
	output: {
		sourcemap: !production,
		dir: 'dist/',
		format: 'esm',
		entryFileNames: '[name][hash].js',
		chunkFileNames: '[name][hash].js',
		assetFileNames: '[name][hash][extname]',
	},
	plugins: [
		svelte({
			preprocess: sveltePreprocess({ sourceMap: !production }),
			compilerOptions: {
				// enable run-time checks when not in production
				dev: !production
			},
			emitCss: false,
		}),
		// we'll extract any component CSS out into
		// a separate file - better for performance
		css({ output: 'bundle.css' }),

		html({
			title: 'Mug',
			meta: [
				{ charset: 'utf-8' },
				{ name:'viewport', content: 'width=device-width,initial-scale=1' },
			],
		}),

		// If you have external dependencies installed from
		// npm, you'll most likely need these plugins. In
		// some cases you'll need additional configuration -
		// consult the documentation for details:
		// https://github.com/rollup/plugins/tree/master/packages/commonjs
		resolve({
			browser: true,
			dedupe: ['svelte']
		}),
		commonjs(),
		typescript({
			sourceMap: !production,
			inlineSources: !production
		}),

		// In dev mode, call `npm run start` once
		// the bundle has been generated
		!production && serve(),

		// Watch the `dist` directory and refresh the
		// browser on changes when not in production
		!production && livereload('dist'),

		// If we're building for production (npm run build
		// instead of npm run dev), minify
		production && terser({
			// format: {
			// 	ecma: 2015,
			// },
			// mangle: {
			// 	toplevel: true,
			// },
			// compress: {
			// 	ecma: 2015,
			// 	toplevel: true,
			// 	hoist_funs: true,
			// 	passes: 3,
			// 	unsafe: true,
			// 	unsafe_comps: true,
			// 	unsafe_Function: true,
			// 	unsafe_math: true,
			// 	unsafe_symbols: true,
			// 	unsafe_methods: true,
			// 	unsafe_proto: true,
			// 	unsafe_regexp: true,
			// 	unsafe_undefined: true,
			// },
			// ecma: 2015,
			// toplevel: true,
		}),
		visualizer(),
	],
	watch: {
		clearScreen: false
	}
};
