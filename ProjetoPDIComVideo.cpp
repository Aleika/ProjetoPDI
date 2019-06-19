#include <iostream>
#include <opencv2/opencv.hpp>

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/utility.hpp>

using namespace std;
using namespace cv;

Size frameSize(1080,1080);
vector<Mat> images;
Mat tela_preta;
VideoWriter out("final/sorrisos.avi",CV_FOURCC('P','I','M','1'), 25, frameSize,true);

CascadeClassifier cascadeFace, cascadeMouth;

String cascadeFaceName  = "haarcascades/haarcascade_frontalface_alt.xml";
String cascadeMouthName = "haarcascades/haarcascade_smile.xml";

void detectSmile(Mat& img);
void saidaVideo(Mat& img);


int main( int argc, const char** argv ){
 
    String folder = "imagensSalvas/*.jpg";
    vector<String> filenames;
    Mat image, img;
    tela_preta = imread("tela_preta.jpg");
  
    if( !cascadeFace.load( cascadeFaceName )) {
	cerr << "ERRO: Nao carregou filtro em cascata facefrontal" << endl;
	return -1;
    }
    
   if( !cascadeMouth.load( cascadeMouthName )) {
	cerr << "ERRO: Nao carregou filtro em cascata mouth" << endl;
	return -1;
    }

    //Função responsável por salva as imagens no diretório especificado
    // em folder
    glob(folder, filenames);

    //são percorridas todas as imagens da pasta
    for (size_t i = 0; i < filenames.size(); ++i)
    {
	//'image' recebe a imagem lida do diretório 
        image = imread(filenames[i]);
      
	detectSmile(image);
    }

  return 0;
}

//Função responsável por detectar o sorriso 
void detectSmile( Mat& img){
  vector<Rect> faces;
  
  cascadeFace.detectMultiScale(img, // imagem para deteccao
				   faces, // vetor com os retangulos encontrados
				   1.1, // escala de multiresolucao
				   3, // numero de vizinhos que cada candidato a retangulo
				   // devera contemplar. evita multiplas deteccoes parecidas
				   // na mesma regiao
				   0 | CV_HAAR_FIND_BIGGEST_OBJECT, // parametros (normalmente nao usados)
				   Size(30, 30) ); // mínimo tamanho para deteccao de um objeto
  
  for( vector<Rect>::const_iterator r = faces.begin(); r != faces.end(); r++ ){
	Mat imgROI;
	vector<Rect> nestedObjects;
	
	
	if( cascadeMouth.empty() )
	  continue;

	// posicao aproximada da boca em relacao a face...
	Rect mouthROI = Rect(r->x, r->y + (r->height/1.5), r->width, r->height/2.5);    


	imgROI = img(mouthROI);
	
	cascadeMouth.detectMultiScale(
					imgROI, // imagem para detectção
					nestedObjects, // vetor com os retangulos encontrados da região da boca
					1.1, // escala de multiresolucao
					10,// numero de vizinhos que cada candidato a retangulo
				           // devera contemplar. evita multiplas deteccoes parecidas
				           // na mesma regiao
					0 | CV_HAAR_FIND_BIGGEST_OBJECT, // parametros (normalmente nao usados)
					Size(30, 30) );// mínimo tamanho para deteccao de um objeto


	for( vector<Rect>::const_iterator nr = nestedObjects.begin(); nr != nestedObjects.end(); nr++ ){
		
		//Para cada imagem é aplicado o efeito e o resultado é 'jogado'
		//diretamente no video
		saidaVideo(img);
			
	}
  }

}

void saidaVideo(Mat &image){
        Mat result; // matriz auxiliar
	float alpha; // variavel que será utilizada no efeito

	//As imagens são redimensionadas para que tenham
	// o mesmo tamanho quando forem adicionadas.
	// O redimensionamento é feito para o tamanho do frame do video.
	resize(tela_preta,tela_preta, frameSize);
        resize(image, image, frameSize);

	// loop que fará a transição da imagem da tela_preta
	// até a imagem 'image'
	for(alpha=0; alpha<=1; alpha+=0.02){
		result = image*alpha + tela_preta*(1-alpha);
		//add(foreground, background, ouImage);	
		out.write(result);
	}

	// A imagem 'image' será mostrada durante 1segundo
	for(int i =0; i<25;i++){
		out.write(image);
	}

	// loop que fará a transição da imagem 'image'
	// até a imagem tela_preta
	for(alpha =1; alpha>=0; alpha-=0.02){
		result = image*alpha + tela_preta*(1-alpha);	
		out.write(result);
	}
}
    
