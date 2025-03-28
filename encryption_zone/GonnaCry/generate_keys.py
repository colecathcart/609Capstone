import base64

# import Crypto.Random 
# replaced with cryptodome
from Crypto.Random import get_random_bytes

def generate_key(bits, encode=False):
    # generated = Crypto.Random.OSRNG.posix.DevURandomRNG()
    byte_length = bits // 8
    
    # content = generated.read(bits)
    content = get_random_bytes(byte_length)

    if(encode):
        return base64.b64encode(content)

    return content

if __name__ == "__main__":
    # print(generate_key(32))
    print(generate_key(128, encode=True))