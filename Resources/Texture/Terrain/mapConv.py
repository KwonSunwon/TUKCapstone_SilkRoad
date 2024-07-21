from PIL import Image
import struct

# 이미지 열기
image = Image.open('height.png')

# 이미지를 흑백으로 변환
image = image.convert('L')

# 이미지의 너비와 높이 가져오기
width, height = image.size

# 각 픽셀 값 얻기
pixels = list(image.getdata())

# 픽셀 값을 바이너리 파일에 저장
with open('pixels10.bin', 'wb') as f:
    for pixel in pixels:
        f.write(struct.pack('B', pixel))  # B format means unsigned char

