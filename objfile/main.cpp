#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

using namespace std;

struct vec3 {
    float x,y,z;
};

int main(int argc, char** argv) {
    string filename = "cube.obj";

    ifstream file(filename.c_str());
    vector<vec3> verts, norms, texts;
    if(!file.is_open()) {
        cerr << "file not open" << endl;
        return -1;
    }
    int vi,ni,ti;

    string trash;
    while(!file.eof()) {
    if(!(file >> trash)) {
    } else if(trash == "vt") {
        float u,v;
        file >> u >> v;
        cout << "vt " << u << " " << v << endl;
    } else if (trash == "vn") {
        vec3 norm;
        file >> norm.x >> norm.y >> norm.z;
        cout << "vn " << norm.x << ", " << norm.y << ", " << norm.z << endl;
    } else if (trash == "v") {
        vec3 vert;
        file >> vert.x >> vert.y >> vert.z;
        cout << "v " << vert.x << ", " << vert.y << ", " << vert.z << endl;
    } else if (trash == "f") {
        unsigned int mode = 0; // 0 = vert, 1 = vert/tex, 2 = vert//norm, 3 = vert/tex/norm
        unsigned int polygon = 0;
        {
            int bol = file.tellg();
            string block;
            file >> block; 
            for (int i = 0; i < block.length(); i++) {
                if(block[i] == '/') {
                    mode++;
                    if (mode == 2) 
                        mode += (block[i-1] == '/') ? 0 : 1;
                }
            }
            file.seekg(bol);
            block.reserve(70);
            getline(file, block);
            stringstream strm(block);
            while(strm >> block) { polygon++; }
            file.seekg(bol);
        }
        if (polygon < 3) {
            cerr << "don't know how to draw a less than 3-sided polygon" << endl;
            return -2;
        }
        vector<vec3> face;
        char spacer;
        vec3 facebuf;
        cout << "m:" << mode << ", p:" << polygon << ", f";
        for(int i = 0; i < polygon; i++) {
            switch (mode) {
            case 0: {
                file >> facebuf.x;
                facebuf.y = facebuf.z = 0;
                break;
            }
            case 1: {
                file >> facebuf.x >> spacer >> facebuf.y;
                facebuf.z = 0;
                break;
            }
            case 2: {
                file >> facebuf.x >> spacer >> spacer >> facebuf.z;
                facebuf.y = 0;
                break;
            }
            case 3: {
                file >> facebuf.x >> spacer >> facebuf.y >> spacer >> facebuf.z;
                break;
            }
            }
            face.push_back(facebuf);
            cout << " " << face[i].x << "/" << face[i].y << "/" << face[i].z;
        }
        cout << endl << "    ";
        for(int i = 2; i < polygon; i++) {
            cout << "{ ";
            cout << face[0].x << "/" << face[0].y << "/" << face[0].z << ", ";
            cout << face[i-1].x << "/" << face[i-1].y << "/" << face[i-1].z << ", ";
            cout << face[i].x << "/" << face[i].y << "/" << face[i].z << " } ";
        }
        cout << endl;
    } else if (trash == "o") {
        string objname;
        file >> objname;
        cout << "o " << objname << endl;
    } else if (trash == "mtllib") {
        string material;
        file >> material;
        cout << "mtllib " << material << endl;
    } else {
        string content;
        getline(file, content);
        cout << "--" << trash << " " << content << endl;
    }
    }
    return 0;
}
