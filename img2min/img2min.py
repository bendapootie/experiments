import math
import heapq
from PIL import Image
from PIL import ImageDraw
from PIL import ImageChops

# Priority Queue example...
# quads = []
# heapq.heappush(quads, (2, "Two"))
# heapq.heappush(quads, (3, "Three"))
# heapq.heappush(quads, (1, "One"))
# heapq.heappush(quads, (4, "Four"))
# while quads:
#     print(heapq.heappop(quads))

def to_int_rgb(rgb):
    return (int(rgb[0]), int(rgb[1]), int(rgb[2]))

# Returns sum of all pixel values in the image
# If the provided image is the difference between two other images, this
# computes a reasonable value for how different they are
def compute_sum(img, quad):
    # TODO: Compute the average difference by component (ie. +/-rgb value)
    sum = 0
    for x in range(quad[0], quad[2]):
        for y in range(quad[1], quad[3]):
            pixel = img.getpixel((x, y))
            for c in pixel:
                sum += c
    return sum

# Returns the average (r,g,b) value of the quad in the image
def compute_average(img, quad):
    count = 0
    rgb_sum = [0, 0, 0]
    for x in range(quad[0], quad[2]):
        for y in range(quad[1], quad[3]):
            pixel = img.getpixel((x, y))
            rgb_sum[0] += pixel[0]
            rgb_sum[1] += pixel[1]
            rgb_sum[2] += pixel[2]
            count += 1

    avg_rgb = (
        rgb_sum[0] / count,
        rgb_sum[1] / count,
        rgb_sum[2] / count
    )
    return avg_rgb

# Input
# - Image to process at
# - Sub-rect of image to process
# - (rgb) color to compare to
# Output
# - Sum of abs delta of image to passed in rgb value
def compute_delta_from_value(img, rect, rgb):
    rgb_sum = [0, 0, 0]
    for x in range(rect[0], rect[2]):
        for y in range(rect[1], rect[3]):
            pixel = img.getpixel((x, y))
            rgb_sum[0] += abs(pixel[0] - rgb[0])
            rgb_sum[1] += abs(pixel[1] - rgb[1])
            rgb_sum[2] += abs(pixel[2] - rgb[2])

    return rgb_sum[0] + rgb_sum[1] + rgb_sum[2]


# Input
# - two rects (x1, y1, x2, y2)
def rects_overlap(r1, r2):
    # If one rectangle is on left side of other 
    if(r1[2] <= r2[0] or r2[2] <= r1[0]):
        return False
  
    # If one rectangle is above other
    if(r1[3] <= r2[1] or r2[3] <= r1[1]):
        return False

    return True

# Input
# - test_rect (improvement, (rect), (rgb))
# - List of (improvement, (rect), (rgb)) to compare to
def test_rect_options_for_overlap(test_rect, rect_list):
    for r in rect_list:
        if rects_overlap(test_rect[1], r[1]):
            return True
    return False

# Inputs
# - Ground truth image
# - In-progress image
# Outputs
# - Next rectangle to draw (improvment, rect, rgb)
def get_next_best_rects(ground_img, progress_img, rect_size, iterations, max_rects_to_return):
    # Compute difference between images
    delta_img = ImageChops.difference(ground_img, progress_img)
    h = ground_img.height
    w = ground_img.width

    # Evaluate different rects to see where the most improvement can happen
    # List of (improvement, (rect), (rgb))
    potential_options = []
    iters = max(iterations, 1)
    x_slices = math.ceil(iters * w / rect_size[0])
    y_slices = math.ceil(iters * h / rect_size[1])
    for y in range(0, y_slices):
        for x in range(0, x_slices):
            x1 = min(x * rect_size[0] / iters, w - rect_size[0] - 1)
            y1 = min(y * rect_size[1] / iters, h - rect_size[1] - 1)
            x2 = x1 + rect_size[0]
            y2 = y1 + rect_size[1]
            
            rect = (int(x1), int(y1), int(x2), int(y2))

            avg_rgb = to_int_rgb(compute_average(ground_img, rect))
            current_delta = compute_sum(delta_img, rect)
            delta_from_avg = compute_delta_from_value(ground_img, rect, avg_rgb)
            improvement = current_delta - delta_from_avg
            potential_options.append((improvement, rect, avg_rgb))
    
    best_options = []
    potential_options.sort()
    while len(potential_options) > 0 and len(best_options) < max_rects_to_return:
        rect = potential_options.pop()
        if (test_rect_options_for_overlap(rect, best_options) == False):
            best_options.append(rect)
    
    return best_options

# Inputs
# - Ground truth image
# - Max number of rectangles to use
# Outputs
# - Ordered list of rectangles (x1,y1,x2,y2) and colors (r,b,g,a)
def build_rectangle_list_from_image(img, max_num_rects):
    out_rects = []

    # New, empty image to be our canvas
    out_img = Image.new('RGB', (img.width, img.height))
    # ImageDraw object to facilitate drawing onto out_img
    out_draw = ImageDraw.Draw(out_img)

    # How many times the image's size should the iterations cover?
    ITERATION_SCALAR = 4.0

    while (len(out_rects) < max_num_rects):
        percent_done = len(out_rects) / max_num_rects
        t = math.pow(percent_done, 2.0)
        denom = 3.0 * (1 - t) + (img.width / 1.5) * (t)
        s = int(img.width / denom)
        size = (s, s)

        max_rects_to_return = min(10, max_num_rects - len(out_rects))
        next_rects = get_next_best_rects(img, out_img, size, ITERATION_SCALAR, max_rects_to_return)

        #out_rects.append(next_rect)
        out_rects.extend(next_rects)
        for r in next_rects:
            out_draw.rectangle(xy=r[1], fill=r[2])
            print(s, r)
    
    out_img.save('out_img.png')

    return out_rects

def print_mindustry_commands(rect_list, img, flush_frequency = 0):
    height = img.height

    print("Mindustry Commands ---------------------------------------------------------")
    print("jump 2 equal done false")
    print("end")
    print("draw clear 0 0 0 0 0 0")
    for i, r in enumerate(rect_list):
        print("draw color {0} {1} {2} 255 0 0".format(r[2][0], r[2][1], r[2][2]))
        x = r[1][0]
        y = (height - 1) - r[1][3]
        # Add 1 to width and height because Mindustry addressing things in slightly different way than pillow
        w = abs(r[1][2] - x) + 1
        h = abs(r[1][1] - r[1][3]) + 1
        print("draw rect {0} {1} {2} {3} 0 0".format(x, y, w, h))
        if (flush_frequency > 0) and ((i + 1) % flush_frequency == 0):
            print("drawflush display1")
    print("drawflush display1")
    print("set done true")
    print("end Mindustry Commands -----------------------------------------------------")

def main():
    dimensions = (80, 80)
    max_rects = 470
    
    #read the image
    image = Image.open("philip_512.jpg")
    image.convert("RGB")
    base_img = image.resize(dimensions, resample = Image.BOX)

    rect_list = build_rectangle_list_from_image(base_img, max_rects)

    print(rect_list)

    print_mindustry_commands(rect_list, base_img, 10)

if __name__ == "__main__":
	main()