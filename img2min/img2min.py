import math
import heapq
import random
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

# Inputs
# - Ground truth image
# - In-progress image
# Outputs
# - Next rectangle to draw (improvment, rect, rgb)
def get_next_best_rect(ground_img, progress_img, max_rect_size, iterations):
    # Compute difference between images
    delta_img = ImageChops.difference(ground_img, progress_img)
    h = ground_img.height
    w = ground_img.width

    # Evaluate different rects to see where the most improvement can happen
    best = (0, (0, 0, 0, 0), (0, 0, 0))
    for x in range(0, iterations):

        x1 = random.randrange(w)
        y1 = random.randrange(h)
        x2 = x1
        y2 = y1
        while x2 == x1 or y2 == y1:
            x2 = random.randrange(w)
            y2 = random.randrange(h)
        rect = (min(x1, x2), min(y1, y2), max(x1, x2), max(y1, y2))
        #x1 = random.randrange(w - max_rect_size[0])
        #y1 = random.randrange(h - max_rect_size[1])
        #rect = (int(x1), int(y1), int(x1 + max_rect_size[0]), int(y1 + max_rect_size[1]))

        avg_rgb = to_int_rgb(compute_average(ground_img, rect))
        current_delta = compute_sum(delta_img, rect)
        delta_from_avg = compute_delta_from_value(ground_img, rect, avg_rgb)
        improvement = current_delta - delta_from_avg
        if (improvement > best[0]):
            best = (improvement, rect, avg_rgb)
            #print(best)

    return best

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
    ITERATION_SCALAR = 1.0

    while (len(out_rects) < max_num_rects):
        percent_done = len(out_rects) / max_num_rects
        t = math.pow(percent_done, 2)
        denom = 2.0 * (1 - t) + img.width * (t)
        s = int(img.width / denom)
        size = (s, s)

        img_pixels = img.width * img.height
        rect_pixels = size[0] * size[1]
        #iterations = int(max(1, ITERATION_SCALAR * img_pixels / rect_pixels))
        iterations = 1000

        next_rect = get_next_best_rect(img, out_img, size, iterations)
        out_rects.append(next_rect)
        out_draw.rectangle(xy=next_rect[1], fill=next_rect[2])
        print(next_rect)
    
    out_img.save('out_img.png')

    return out_rects

def print_mindustry_commands(rect_list, img):
    height = img.height

    print("Mindustry Commands ---------------------------------------------------------")
    print("draw clear 0 0 0 0 0 0")
    for r in rect_list:
        print("draw color {0} {1} {2} 255 0 0".format(r[2][0], r[2][1], r[2][2]))
        x = r[1][0]
        y = (height - 1) - r[1][3]
        w = abs(r[1][2] - x)
        h = abs(r[1][1] - r[1][3])
        print("draw rect {0} {1} {2} {3} 0 0".format(x, y, w, h))
    print("drawflush display1")
    print("end Mindustry Commands -----------------------------------------------------")

def main():
    dimensions = (80, 80)
    max_rects = 100
    random.seed(10)

    #read the image
    image = Image.open("philip_512.jpg")
    image.convert("RGB")
    base_img = image.resize(dimensions, resample = Image.BOX)

    rect_list = build_rectangle_list_from_image(base_img, max_rects)

    print(rect_list)

    print_mindustry_commands(rect_list, base_img)

if __name__ == "__main__":
	main()