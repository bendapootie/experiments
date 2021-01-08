import math
import heapq
from PIL import Image
from PIL import ImageDraw
from PIL import ImageChops

# Basically a naive box filter that only works with integral scales
def resize_average(image, new_width, new_height):
    out_img = Image.new('RGB', (new_width, new_height), color = (0, 0, 0))
    raw = out_img.load()
    print(out_img.size)
    pixels_per_x = image.size[0] / new_width
    pixels_per_y = image.size[1] / new_height
    print(f"pix/x = {pixels_per_x} pix/y = {pixels_per_y}")
    for y in range(out_img.size[1]):
        for x in range(out_img.size[0]):
            sum = [0,0,0]
            for j in range(int(pixels_per_y)):
                for i in range(int(pixels_per_x)):
                    xi = (x * pixels_per_x) + i
                    yj = (y * pixels_per_y) + j
                    src = image.getpixel((xi, yj))
                    if (x == 1 and y == 0 or True):
                        print("{:4}".format(int((src[0] + src[1] + src[2]) / (3.0))), end = "")
                    sum[0] += src[0]
                    sum[1] += src[1]
                    sum[2] += src[2]
                if (x == 1 and y == 0 or True):
                    print("")
            sum[0] /= (pixels_per_y * pixels_per_x)
            sum[1] /= (pixels_per_y * pixels_per_x)
            sum[2] /= (pixels_per_y * pixels_per_x)
            print(sum)
        print()
    return out_img

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

def main():
    dimensions = (80, 80)

    #read the image
    image = Image.open("philip_512.jpg")
    image.convert("RGB")

    base_img = image.resize(dimensions, resample = Image.BOX)
    out_img = Image.new('RGB', dimensions)
    out_draw = ImageDraw.Draw(out_img)

    delta_img = ImageChops.difference(base_img, out_img)

    # Priority Queue example...
    # quads = []
    # heapq.heappush(quads, (2, "Two"))
    # heapq.heappush(quads, (3, "Three"))
    # heapq.heappush(quads, (1, "One"))
    # heapq.heappush(quads, (4, "Four"))
    # while quads:
    #     print(heapq.heappop(quads))
    
    quads = []
    quads.append((0, 0, dimensions[0], dimensions[1]))
    delta_sum = compute_sum(delta_img, quads[0])
    avg_rgb = compute_average(base_img, quads[0])
    print(delta_sum)
    print(avg_rgb)
    rgb = (math.floor(avg_rgb[0]), math.floor(avg_rgb[1]), math.floor(avg_rgb[2]))
    out_draw.rectangle(quads[0], fill=rgb)

    out_img.save('out_img.png')

    # print("Ascii characters and their associated average rgb values based on 'ascii_chars.png'")
    # for y in range(6):
    #     for x in range(16):
    #         c = new_image.getpixel((x, y))
    #         grey = int((c[0] + c[1] + c[2]) / 3.0)
    #         print("{0} = {1}".format((chr(32 + (y * 16) + x)), grey))

if __name__ == "__main__":
	main()