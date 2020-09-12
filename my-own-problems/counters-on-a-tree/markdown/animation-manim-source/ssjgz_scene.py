from manimlib.imports import *

# A standard Scene with a few extra bells-and-whistles: mainly
# the begin_background_animations and the corresponding override
# of play() which allows one to start an animation "in the background"
# i.e. you can start a long-running background animation B and when play() is called for
# some other animation(s), B will play for the duration of those animations and
# then pause until the next time play() is called.
#
# Also allows the scene size (in pixels) and the frame rate to be set via the MANIM_CAMERA
# environment variable.
class SSJGZScene(Scene):
    CONFIG={
            "camera_config": {
                "background_color":WHITE, 
            }
    }

    def construct(self):
        self.background_anims = []
        if 'MANIM_CAMERA' in os.environ:
            camera_params = os.environ['MANIM_CAMERA'].split(',')
            print("camera_params:", camera_params)
            self.camera_config['pixel_width'] = int(camera_params[0])
            self.camera_config['pixel_height'] = int(camera_params[1])
            self.camera_config['frame_rate'] = int(camera_params[2])
            self.camera = self.camera_class(**self.camera_config)


    def play(self, *args, **kwargs):

        animations = self.compile_play_args_to_animation_list(
            *args, **kwargs
        )
        args = list(args)
        non_background_runtime = self.get_run_time(animations)
        print("Play: non_background_runtime:", non_background_runtime)
        for anim in self.background_anims:
            print(" anim: remaining_run_time:", anim.remaining_run_time, repr(anim))
            anim.run_time = min(non_background_runtime, anim.remaining_run_time)
            anim.this_slice_runtime = anim.run_time
            if anim.run_time >= 0:
                args.append(anim)

        super().play(*args, **kwargs)

        for anim in self.background_anims:
            print("non_background_runtime:", non_background_runtime)
            anim.remaining_run_time = anim.remaining_run_time - non_background_runtime

        self.background_anims = [a for a in self.background_anims if a.remaining_run_time > 0]

    def begin_background_animations(self, *background_anims):
        for anim in background_anims:
            self.background_anims.append(anim)

    def save_thumbnail(self):
        Image.fromarray(self.get_frame()).save(self.__class__.__name__ + '_thumbnail.png')
