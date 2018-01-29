# coding=utf-8
from werkzeug.utils import secure_filename
from flask import Flask,render_template,jsonify,request,send_from_directory
import time
import os
import base64
import mapOptimize
from ctypes import *
 
app = Flask(__name__)
UPLOAD_FOLDER='upload'
app.config['UPLOAD_FOLDER'] = UPLOAD_FOLDER
basedir = os.path.abspath(os.path.dirname(__file__))
ALLOWED_EXTENSIONS = set(['txt','png','jpg','xls','JPG','PNG','xlsx','gif','GIF'])
# class MapSize(Structure):
#     _fields_=[('minX',c_uint),
#               ('minY',c_uint),
#               ('maxX',c_uint),
#               ('maxY',c_uint),]
class MapSize:
    def __init__(self):
        self.minX = 0
        self.minY = 0
        self.maxX = 0
        self.maxY = 0

# 用于判断文件后缀
def allowed_file(filename):
    return '.' in filename and filename.rsplit('.',1)[1] in ALLOWED_EXTENSIONS
 
# 上传文件
@app.route('/api/upload',methods=['POST'],strict_slashes=False)
def api_upload():
    print 'start time:',time.time()
    # mapSize = MapSize()
    # mapSize.minX = int(request.values.get('minX', 0))
    # mapSize.maxX = 0
    # mapSize.minY = 0
    # mapSize.maxY = 0
    minX = int(request.values.get('minX', 0))
    maxX = int(request.values.get('maxX', 0))
    minY = int(request.values.get('minY', 0))
    maxY = int(request.values.get('maxY', 0))
    robotID = str(request.values.get('robotId', ''))
    print minX,maxX,minY,maxY,robotID
    file_dir=os.path.join(basedir,app.config['UPLOAD_FOLDER'])
    if not os.path.exists(file_dir):
        os.makedirs(file_dir)
    f=request.files['file']  # 从表单的file字段获取文件，myfile为该表单的name值
    print f.filename
    if f and allowed_file(f.filename):  # 判断是否是允许上传的文件类型
        fname=secure_filename(f.filename)
        print fname
        ext = fname.rsplit('.',1)[1]  # 获取文件后缀
        unix_time = int(time.time())
        new_filename= 'ultrasonic_sla.txt'#str(unix_time)+'.'+ext  # 修改了上传的文件名
        f.save(os.path.join(file_dir,new_filename))  #保存文件到upload目录
        token = base64.b64encode(new_filename)
        print token
        ret = mapOptimize.mapOptimizeOffline(os.path.join(file_dir,new_filename),
                                             os.path.join(file_dir,"aaabbb.bin"),
                                             minX,minY,maxX,maxY)#mapSize
        if not ret:
            return jsonify({"errno": 1001, "errmsg": "上传失败"})
        print 'end time:', time.time()
        return send_from_directory(file_dir,"aaabbb.bin",as_attachment=True)
    else:
        return jsonify({"errno":1001,"errmsg":"上传失败"})
@app.route('/api/download')
def download():
    file_dir = os.path.join(basedir, app.config['UPLOAD_FOLDER'])
    return send_from_directory(file_dir, 'ultrasonic_sla.txt', as_attachment=True)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)#debug=True
