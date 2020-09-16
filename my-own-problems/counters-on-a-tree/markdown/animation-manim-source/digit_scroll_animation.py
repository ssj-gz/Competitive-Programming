from manimlib.imports import *

# Helpers for creating the "change digit to a new one by scrolling it"
# animation.
class DigitVGroup(VGroup):
    def __init__(self, color = BLACK, scale = 1):
        self.digits = []
        for digit in range(10):
            digitMObject = TexMobject(str(digit))
            digitMObject.scale(scale)
            digitMObject.set_color(color)
            self.digits += digitMObject

        VGroup.__init__(self,
                        *self.digits
                       )

        for digit,digitMObject in enumerate(self.digits):
            # Stack vertical with tight-ish packing.
            if(digit != 0):
                digitMObject.next_to(self.digits[digit-1],DOWN * 0.6)

        self.initial_digit_pos = {}
        for digit,digitMObject in enumerate(self.digits):
            self.initial_digit_pos[digit] = digitMObject.get_center() - self.get_center()



class DigitVGroupMoveAnim(Animation):
    def __init__(self, digitVGroup, start_pos, end_pos, actual_center, dist_from_center_before_invisible, original_digitMobject, replacement_digitMobject, x_move_amount, *args):
        self.digitVGroup = digitVGroup
        self.start_pos = start_pos
        self.end_pos = end_pos
        self.actual_center = actual_center
        self.dist_from_center_before_invisible = dist_from_center_before_invisible
        self.original_digitMobject = original_digitMobject
        self.replacement_digitMobject = replacement_digitMobject
        Animation.__init__(self, digitVGroup,
                        *args
                       )
        self.remover = True
        self.x_move_amount = x_move_amount
        print("x_move_amount:", x_move_amount)

    def interpolate_mobject(self, alpha):
       if alpha == 0:
           self.original_digitMobject.set_opacity(0)
       if alpha == 1:
            self.original_digitMobject.become(self.replacement_digitMobject)
            self.original_digitMobject.set_opacity(1)

       self.digitVGroup.move_to(self.start_pos + (self.end_pos - self.start_pos) * (alpha) + self.x_move_amount * alpha * RIGHT)
       for digit,digitMObject in enumerate(self.digitVGroup.digits):
           digit_position = self.digitVGroup.get_center() + self.digitVGroup.initial_digit_pos[digit]
           dist_from_actual_center = abs(digit_position[1] - self.actual_center[1])
           new_opacity = 0.0
           if dist_from_actual_center < self.dist_from_center_before_invisible:
               new_opacity = interpolate(1, 0.0, dist_from_actual_center / self.dist_from_center_before_invisible)

           digitMObject.set_opacity(new_opacity)


# We can't detect the amount the initial_digitMObject is scaled by nor which digit it represents, so these need
# to be passed in as digitMObjectScale and initial_digit, respectively.
def create_scroll_digit_to_animation(initial_digitMObject, initial_digit, digit_to_scroll_to, digitMObjectScale = 1, x_move_amount = 0, y_move_amount = 0):
    print("initial_digit:", initial_digit)

    digitVGroup = DigitVGroup(color = initial_digitMObject.get_color(), scale = digitMObjectScale)

    offset = digitVGroup.initial_digit_pos[initial_digit]# + (0, digitVGroup.get_height() / 2, 0)
    digitVGroup.move_to(initial_digitMObject.get_center() - offset)

    # The initial_digitMObject will instantly "become" replacement_digitMobject when the animation is complete.
    replacement_DigitMObject = TexMobject(str(digit_to_scroll_to))
    replacement_DigitMObject.scale(digitMObjectScale)
    replacement_DigitMObject.set_color(initial_digitMObject.get_color())
    replacement_DigitMObject.move_to(initial_digitMObject.get_center() + x_move_amount * RIGHT)
    replacement_DigitMObject.set_opacity(0)

    amount_to_scroll_by = (digitVGroup.digits[initial_digit].get_center() - digitVGroup.digits[digit_to_scroll_to].get_center())
    fully_opaque_at_pos = digitVGroup.get_center() + digitVGroup.initial_digit_pos[initial_digit]
    dist_from_center_before_invisible = digitVGroup.digits[digit_to_scroll_to].get_height()
    return DigitVGroupMoveAnim(digitVGroup, digitVGroup.get_center(), digitVGroup.get_center() + amount_to_scroll_by, fully_opaque_at_pos, dist_from_center_before_invisible, initial_digitMObject, replacement_DigitMObject, x_move_amount)


