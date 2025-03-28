import base64
import hashlib

import generate_keys

from Crypto import Random
from Crypto.Cipher import AES
# import generate_keys

class AESCipher(object):

    def __init__(self, key): 
        self.bs = 32
        self.key = hashlib.sha256(key).digest()

    def encrypt(self, raw):
        
        # added conversion of input from str to bytes  
        if isinstance(raw, str):
            raw = raw.encode('utf-8')
        
        raw = self._pad(raw)
        iv = Random.new().read(AES.block_size)
        cipher = AES.new(self.key, AES.MODE_CBC, iv)
        return base64.b64encode(iv + cipher.encrypt(raw))

    def decrypt(self, enc, decryption_key=None):
        enc = base64.b64decode(enc)
        iv = enc[:AES.block_size]
        if(decryption_key):
            self.key = hashlib.sha256(decryption_key).digest()
            
        cipher = AES.new(self.key, AES.MODE_CBC, iv)
        return self._unpad(cipher.decrypt(enc[AES.block_size:]))

    # def _pad(self, s):
    #     s = s.decode('utf-8')
    #     return s + (self.bs - len(s) % self.bs) * chr(self.bs - len(s) % self.bs)

    def _pad(self, data):
        pad_len = self.bs - len(data) % self.bs
        return data + bytes([pad_len] * pad_len)
    
    @staticmethod
    # def _unpad(s):
    #     return s[:-ord(s[len(s)-1:])]
    def _unpad(data):
        pad_len = data[-1]
        return data[:-pad_len]

if __name__ == "__main__":
    # key = generate_keys.generate_key(32, True)
    key = generate_keys.generate_key(128, True)
    cipher_obj = AESCipher(key)
    print("chave: {}".format(key))
    enc = cipher_obj.encrypt("TESTE CRYPTO")
    print(enc)

    back = cipher_obj.decrypt(enc, key)

    print(back)