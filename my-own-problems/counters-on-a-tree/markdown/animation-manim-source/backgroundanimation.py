from manimlib.imports import *

# For use with SSJGZScene.begin_background_animations.
class BackgroundAnimation(AnimationGroup):
    def __init__(self, *animations, **kwargs):
        AnimationGroup.__init__(self, *animations, **kwargs)
        self.original_run_time = self.run_time
        self.remaining_run_time = self.run_time
        self.this_slice_runtime = 0
        self.begin_called = False
        print("gobbles: original_run_time:", self.original_run_time)

    def interpolate(self, alpha):
        total_time_passed_so_far = self.original_run_time - self.remaining_run_time + alpha * self.this_slice_runtime
        adjusted_alpha = total_time_passed_so_far / self.original_run_time
        print(" interpolate alpha:", alpha, " total_time_passed_so_far:", total_time_passed_so_far, " original_run_time:", self.original_run_time, " adjusted_alpha:", adjusted_alpha)
        super().interpolate(adjusted_alpha)

    def begin(self):
        if self.begin_called:
            print("Ignoring call to begin")
            return

        print("Processing call to begin")
        self.begin_called = True
        super().begin()

    def finish(self):
        total_time_passed_so_far = self.original_run_time - self.remaining_run_time + self.this_slice_runtime
        print("finish: total_time_passed_so_far:", total_time_passed_so_far, " original_run_time:", self.original_run_time)
        if total_time_passed_so_far < self.original_run_time:
            print("ignoring call to finish()")
            return

        print("processing call to finish()")
        super().finish()


