# coding=utf-8
from werkzeug.utils import secure_filename
from flask import Flask,render_template,jsonify,request,send_from_directory
import time
import os
import base64
import mapOptimize
 
app = Flask(__name__)
UPLOAD_FOLDER='upload'
app.config['UPLOAD_FOLDER'] = UPLOAD_FOLDER
basedir = os.path.abspath(os.path.dirname(__file__))
ALLOWED_EXTENSIONS = set(['txt','png','jpg','xls','JPG','PNG','xlsx','gif','GIF'])
 
# 用于判断文件后缀
def allowed_file(filename):
    return '.' in filename and filename.rsplit('.',1)[1] in ALLOWED_EXTENSIONS
 
# 上传文件
@app.route('/api/upload',methods=['POST'],strict_slashes=False)
def api_upload():
    print 'start time:',time.time()
    file_dir=os.path.join(basedir,app.config['UPLOAD_FOLDER'])
    if not os.path.exists(file_dir):
        os.makedirs(file_dir)
    print '$$$$$$$$$$$$',request.files
    f=request.files['myfile']  # 从表单的file字段获取文件，myfile为该表单的name值
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
                                             os.path.join(file_dir,"aaabbb.bin"))
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
    app.run(debug=True)
