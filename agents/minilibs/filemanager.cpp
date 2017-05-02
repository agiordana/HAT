//
//  FileManager.cpp
//  FileManager
//
//  Created by Davide Monfrecola on 12/7/11.
//  Copyright 2011 Università del Piemonte Orientale. All rights reserved.
//

#include "agentlib.h"

using namespace std;

string FileManager::basedir = "";

bool FileManager::makeDir(string &dirpath, bool perror_flag) {
    
    if (!(isDir(dirpath))) {
        
        if(mkdir((dirpath).c_str(), 0755) == 0) {
            return true;
        }
        else {
            
            if(perror_flag) {
                hsrv::logger->fatal("[Filemanager::createDir] error creating dir (mkdir): " + dirpath, __FILE__, __FUNCTION__, __LINE__);
                perror("Impossible to create dir (see also log): ");
                exit(-1);
            }
            else {
                hsrv::logger->error("[Filemanager::createDir] error creating dir (mkdir): " + dirpath, __FILE__, __FUNCTION__, __LINE__);
            }
            
            return false;
        }
        
    }
    // directory già esistente
    return true;
    
}

bool FileManager::deleteDir(string &dirpath) {
    
    DIR *dirp;
	struct dirent *dp;
	vector<string> dlist;
    
	dirp = opendir((dirpath).c_str());
	
    if(dirp == NULL) {
        hsrv::logger->error("[Filemanager::deleteDir] error opening dir : " + dirpath, __FILE__, __FUNCTION__, __LINE__);
        return false;
    }
	
    while((dp = readdir(dirp)) != NULL) {
        string cur_filename(dp->d_name);
        
        if ((cur_filename != ".") && (cur_filename != "..")) {
            // trovata directory, chiamata ricorsiva
            if (dp->d_type == DT_DIR) {
                string dirname(dp->d_name);
                string sub_dirname(dirpath+"/"+dirname);
                
                if(!(FileManager::deleteDir(sub_dirname))) {
                    hsrv::logger->error("[Filemanager::deleteDir] error deleting subdir (FileManager::deleteDir): " + sub_dirname, __FILE__, __FUNCTION__, __LINE__);
                    return false;
                }

            }
            // altrimenti elimino il file
            else {
                
                if(!(FileManager::deleteFile(dirpath, cur_filename))) {
                    hsrv::logger->error("[Filemanager::deleteDir] error deleting file (FileManager::deleteFile): " + dirpath + "/" + cur_filename, __FILE__, __FUNCTION__, __LINE__);
                    return false;
                }

            }
            
        }
    }
    // elimino directory corrente
    if(rmdir((dirpath).c_str()) == -1) {
        hsrv::logger->error("[Filemanager::deleteDir] error deleting dir (rmdir): " + dirpath, __FILE__, __FUNCTION__, __LINE__);
        return false;
    }
    
    closedir(dirp);
    
    return true;
    
}

bool FileManager::deleteFile(string &filepath, const string &filename) {
    
    string complete_filepath("");
    
    if(filename != "") {
        complete_filepath = filepath+"/"+filename;
    }
    else {
        complete_filepath = filepath;
    }
    
    if (isFile(complete_filepath)) {
        if(unlink(complete_filepath.c_str()) == 0) {
            return true;
        }
        else {
            hsrv::logger->error("[Filemanager::deleteFile] error deleting file: " + filepath, __FILE__, __FUNCTION__, __LINE__);
            return false;
        }
        
    }
    else {
        return false;
    }
    
}

bool FileManager::isFile(string &filepath) {
    
    fstream in;
    
    in.open(filepath.c_str());
    
    if(in.good()) {
        in.close();
        return 1;
    }
    else {
//        hsrv::logger->info("[Filemanager::isFile] invalid file: " + filepath, __FILE__, __FUNCTION__, __LINE__);
        return 0;
    }
}

bool FileManager::isFile(string &filepath, const string &fileext) {
    
	fstream in(filepath.c_str());
    
	if(in.good()) {
        
        if(filepath.find(fileext) == string::npos) 
            return 0;
        else 
            return 1;
        
    }
    else {
        hsrv::logger->info("[Filemanager::isFile] invalid file: " + filepath, __FILE__, __FUNCTION__, __LINE__);
        return 0;
    }
    
}

bool FileManager::isDir(string &dirpath) {
    
    DIR *dirp;
	vector<string> dlist;
    
	dirp = opendir((dirpath).c_str());
	
    if(dirp == NULL) {
        hsrv::logger->error("[Filemanager::isDir] error opening directory: " + dirpath, __FILE__, __FUNCTION__, __LINE__);
        return false;
    }
    else {
	closedir(dirp);
        return true;
    }
    
}

int FileManager::fileCount(string& dirpath) {
    
    int filenumber = 0;
    struct dirent *dirp;
    DIR *dp;
    
    if((dp = opendir(dirpath.c_str())) == NULL) {
        hsrv::logger->error("[Filemanager::fileCount] error opening directory: " + dirpath, __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    
    while ((dirp = readdir(dp)) != NULL) {
        string filename(dirp->d_name);
        
        if((filename != ".") && (filename != "..") && (dirp->d_type != DT_DIR)) {
            filenumber++;
        }
        
    }
    
    closedir(dp);
    
    return filenumber;
    
}

int FileManager::fileList(string &dirpath, vector<string> &filelist) {
    
    int filenumber = 0;
    struct dirent *dirp;
    DIR *dp;
    
    if((dp = opendir(dirpath.c_str())) == NULL) {
        hsrv::logger->error("[Filemanager::fileList] error opening directory: " + dirpath, __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    
    while ((dirp = readdir(dp)) != NULL) {
        string filename(dirp->d_name);
        
        if((filename != ".") && (filename != "..") && (dirp->d_type != DT_DIR) && (filename[0] != '.')) {
            filenumber++;
            filelist.push_back(string(dirp->d_name));
        }
    }
    
    closedir(dp);
    
    return filenumber;
    
}

int FileManager::dirCount(std::string &dirpath) {
    
    int dirnumber = 0;
    struct dirent *dirp;
    DIR *dp;
    
    if((dp = opendir(dirpath.c_str())) == NULL) {
        hsrv::logger->error("[Filemanager::dirCount] error opening directory: " + dirpath, __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    
    while ((dirp = readdir(dp)) != NULL) {
        string dir_name(dirp->d_name);
        
        if((dir_name != ".") && (dir_name != "..") && (dirp->d_type == DT_DIR)) {
            dirnumber++;
        }
        
    }
    
    closedir(dp);
    
    return dirnumber;
    
}

int FileManager::dirList(string &dirpath, vector<string> &dirlist) {
    
    int dirnumber = 0;
    struct dirent *dirp;
    DIR *dp;
    
    if((dp = opendir(dirpath.c_str())) == NULL) {
        hsrv::logger->error("[Filemanager::dirList] error opening directory: " + dirpath, __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    
    while ((dirp = readdir(dp)) != NULL) {
        string dir_name(dirp->d_name);
        
        if((dir_name != ".") && (dir_name != ".." && dir_name[0] != '.') && (dirp->d_type == DT_DIR)) {
            dirnumber++;
            dirlist.push_back(string(dirp->d_name));
        }
        
    }
    
    closedir(dp);
    
    return dirnumber;
    
}

long FileManager::filesize(std::string &filepath, const std::string &filename) {
    
    struct stat stat_buf;
    
    if(filename != "") {
        filepath += "/" + filename;
    }
    
    // se il file è valido e la chiamata a stat va a buon fine ritorna la dimensione del file
    // altrimenti ritorna -1
    if((isFile(filepath)) && (stat(filepath.c_str(), &stat_buf) == 0)) {
        return stat_buf.st_size;
    }
    else {
        hsrv::logger->error("[Filemanager::filesize] invalid file: " + filepath, __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    
}

bool FileManager::copyFile(string &filepath, string &new_filepath, const string &new_filename) {
    
    string output_filePath(new_filepath);
    
    if(new_filename != "") {
        output_filePath += "/" + new_filename;
    }
    
    ifstream initial_file(filepath.c_str(), ios::in|ios::binary);
	ofstream output_file(output_filePath.c_str(), ios::out|ios::binary);
	// definisce dimensione buffer
	initial_file.seekg(0, ios::end);
	long fileSize = initial_file.tellg();
    
	// Controlla che entrambi i file siano aperti
	if(initial_file.is_open() && output_file.is_open())
	{
		char * buffer = new char[fileSize];
		// Inizia a copiare il file dall'inizio
		initial_file.seekg(0, ios::beg);
		// copia fino a rimepire il buffer 
		initial_file.read((char*)buffer, fileSize);
		// scrive sul nuovo file creato
		output_file.write((char*)buffer, fileSize);
		delete[] buffer;
	}
	// controlla che non ci siano stati errori durante la copia del file
	else if(!output_file.is_open())
	{
        hsrv::logger->error("[Filemanager::copyFile] error copying file (file output problem): " + output_filePath, __FILE__, __FUNCTION__, __LINE__);
        return false;
	}
	else if(!initial_file.is_open())
	{
        hsrv::logger->error("[Filemanager::copyFile] error copying file (file input problem): " + filepath, __FILE__, __FUNCTION__, __LINE__);
        return false;
	}
    
	initial_file.close();
	output_file.close();
    
    return true;
    
}

bool FileManager::moveFile(string &filepath, string &new_filepath, const string &new_filename) {
    
    string complete_new_filepath(new_filepath);
    
    // controllo se l'utente ha specificato o meno un nuovo nome per il file
    if(new_filename != "") { 
        complete_new_filepath += "/" + new_filename;
    }
    
    if(isFile(filepath)) {
        if(rename(filepath.c_str(), complete_new_filepath.c_str()) == 0) {
            return true;
        }
        else {
            hsrv::logger->error("[Filemanager::moveFile] error moving file (rename): " + filepath, __FILE__, __FUNCTION__, __LINE__);
            return false;
        }
        
    }
    else {
        hsrv::logger->error("[Filemanager::moveFile] error moving file (isFile): " + filepath, __FILE__, __FUNCTION__, __LINE__);
        return false;
    }
    
}

bool FileManager::copyAllFile(string &dirpath, string &new_dirpath, const string &fileext) {
    
    DIR *dirp;
	struct dirent *dp;
    
	dirp = opendir((dirpath).c_str());
	
    if(dirp == NULL) {
        hsrv::logger->error("[Filemanager::copyAllFile] error opening directory: " + dirpath, __FILE__, __FUNCTION__, __LINE__);
        return false;
    }
	
    while((dp = readdir(dirp)) != NULL) {
        string cur_filename(dp->d_name);
        
        if ((cur_filename != ".") && (cur_filename != "..")) {
            
            // trovata directory, chiamata ricorsiva
            if (dp->d_type != DT_DIR) {
                string filename(dp->d_name);
                
                if(fileext == "") {
                    if(!(FileManager::copyFile(dirpath, filename, new_dirpath))) {
                        hsrv::logger->error("[Filemanager::copyAllFile] error copying file: " + dirpath+"/"+filename, __FILE__, __FUNCTION__, __LINE__);
                        return false;
                    }
                }
                else {
                    string complete_filepath = dirpath+"/"+filename;
                    // controlla che il file abbia l'estensione richiesta. Se corrisponde 
                    // procede con la copia del file 
                    if(isFile(complete_filepath, fileext)) {
                        
                        if(!(FileManager::copyFile(dirpath, filename, new_dirpath))) {
                            hsrv::logger->error("[Filemanager::copyAllFile] error copying file: " + complete_filepath, __FILE__, __FUNCTION__, __LINE__);
                            return false;
                        }
                        
                    }
                    
                }
            }
            
        }
    }
    
    closedir(dirp);
    
    return true;
    
}

bool FileManager::moveAllFile(string &dirpath, string &new_dirpath, const string &file_ext) {
    
    DIR *dirp;
	struct dirent *dp;
    
	dirp = opendir((dirpath).c_str());
	
    if(dirp == NULL) {
        hsrv::logger->error("[Filemanager::moveAllFile] error opening directory: " + dirpath, __FILE__, __FUNCTION__, __LINE__);
        return false;
    }
	
    while((dp = readdir(dirp)) != NULL) {
        string cur_filename(dp->d_name);
        
        if ((cur_filename != ".") && (cur_filename != "..")) {
            
            // trovata directory, chiamata ricorsiva
            if (dp->d_type != DT_DIR) {
                string filename(dp->d_name);
                // se non è stata specificata l'estensione copia tutti i file nella directory
                if(file_ext == "") {
                    if(!(FileManager::moveFile(dirpath, filename, new_dirpath))) {
                        hsrv::logger->error("[Filemanager::moveAllFile] error moving file: " + dirpath+"/"+filename, __FILE__, __FUNCTION__, __LINE__);
                        return false;
                    }
                }
                else {
                    string complete_filepath = dirpath+"/"+filename;
                    // controlla che il file abbia l'estensione richiesta. Se corrisponde 
                    // procede con la copia del file 
                    if(isFile(complete_filepath, file_ext)) {
                        
                        if(!(FileManager::moveFile(dirpath, filename, new_dirpath))) {
                            hsrv::logger->error("[Filemanager::moveAllFile] error moving file: " + complete_filepath, __FILE__, __FUNCTION__, __LINE__);
                            return false;
                        }
                        
                    }
                    
                }
            }
            
        }
    }
    
    closedir(dirp);
    
    return true;
}

string FileManager::readFile(string& name) {
	ifstream in;
	string r="";
	string tmp;
    
	in.open(name.c_str());
	if(in == NULL) 
        return r;
    
	while(!in.eof()) {
		in >> tmp;
        
		if(!in.eof()) 
            r = r + tmp;
	}
	in.close();
	return r;
}

bool FileManager::saveFile(std::string& fname, std::string& content) {
	ofstream out;
	out.open(fname.c_str());
	if(out==NULL) {
		hsrv::logger->error(fname+" is not accessible", __FILE__,__FUNCTION__,__LINE__);
		return false;
	}
	out<<content<<endl;
	out.close();
	return true;
}

string FileManager::getTail(string path) {
	int pos;
	
	pos = path.find_last_of('/') + 1;
	if (pos != string::npos) {
		return path.substr(pos, path.size());
	} else {
		return path.substr(0, path.size());
	}
}

string FileManager::getRoot(string path) {
	size_t pos;
        pos = path.find_last_of('/');
	if(pos != string::npos) {
	   return path.substr(0, pos+1);
	} else {
	   return "";
	}
}

string FileManager::getExt(string path) {
	int pos;
	string fname;
	
	fname = FileManager::getTail(path);
	if (fname == "") {
		fname = path;
	}
	
	pos = fname.find_last_of('.') + 1;
	if (pos != string::npos) {
		return fname.substr(pos, path.size());
	} else {
		return fname.substr(0, path.size());
	}
}

string FileManager::getStem(string path) {
	int pos;
	string fname;
	fname = FileManager::getTail(path);
	if (fname == "") {
		fname = path;
	}
	
	pos = fname.find_last_of('.');
	if (pos != string::npos) {
		return fname.substr(0, pos);
	}
	else {
		return fname;
	}
}

bool FileManager::isVideoFile(string path) {
	if (hsrv::isfile(path)) {
		string fname = FileManager::getTail(path);
		vector<string> video_ext;
		video_ext.push_back("MJPEG");
		video_ext.push_back("AVI");
		
		for(int i = 0; i < video_ext.size(); ++i) {
			hsrv::upcase(fname);
			if (hsrv::matchfile(fname, video_ext.at(i))) {
				return true;
			}
		}
	}
	
	return false;
}

bool FileManager::isImageFile(string path) {
	if (hsrv::isfile(path)) {
		string fname = FileManager::getTail(path);
		vector<string> image_ext;
		image_ext.push_back("JPEG");
		image_ext.push_back("JPG");
		image_ext.push_back("PNG");
		image_ext.push_back("GIF");
		
		for(int i = 0; i < image_ext.size(); ++i) {
			hsrv::upcase(fname);
			if (hsrv::matchfile(fname, image_ext.at(i))) {
				return true;
			}
		}
	} 
	
	return false;
}

string FileManager::stripCharacter(string str, char ch) {
    string tmp="";
    for(size_t i=0; i<str.size(); i++)
        if(str[i]!=ch) tmp+=str[i];
    return tmp;
}

