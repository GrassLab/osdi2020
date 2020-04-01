from PIL import Image
 
img = Image.open("123.jpg")
img = img.rotate(90)
img.show()
pixel = img.load()
print (img.size[0],img.size[1])
p_string = ""
for x in range(img.size[0]):
    p_string =  p_string + '\x22'
    for y in range(img.size[1]):
        r,g,b = pixel[x,y]
        p_string = p_string +"\\x" + hex(r)[2:] + "\\x"+ hex(g)[2:]  + "\\x" + hex(b)[2:] 
    p_string =  p_string + '\x22'
    print p_string
    p_string = ""