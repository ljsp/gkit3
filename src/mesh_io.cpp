
#include <cstdlib>
#include <cassert>

#include "mesh_io.h"

#include "image.h"
#include "image_io.h"


bool read_positions( const char *filename, std::vector<Point>& positions )
{
    positions.clear();
    
    FILE *in= fopen(filename, "rt");
    if(!in)
    {
        printf("[error] loading mesh '%s'...\n", filename);
        return false;
    }
    
    printf("loading mesh '%s'...\n", filename);
    
    std::vector<Point> wpositions;
    std::vector<int> windices;
    
    char line_buffer[1024];
    bool error= true;
    for(;;)
    {
        // charge une ligne du fichier
        if(!fgets(line_buffer, sizeof(line_buffer), in))
        {
            error= false;       // fin du fichier, pas d'erreur detectee
            break;
        }
        
        // force la fin de la ligne, au cas ou
        line_buffer[sizeof(line_buffer) -1]= 0;
        
        // saute les espaces en debut de ligne
        char *line= line_buffer;
        while(*line && isspace(*line))
            line++;
        
        if(line[0] == 'v')
        {
            float x, y, z;
            if(line[1] == ' ')          // position x y z
            {
                if(sscanf(line, "v %f %f %f", &x, &y, &z) != 3)
                    break;
                wpositions.push_back( Point(x, y, z) );
            }
        }
        
        else if(line[0] == 'f')         // triangle a b c, les sommets sont numerotes a partir de 1 ou de la fin du tableau (< 0)
        {
            windices.clear();
            
            int next= 0;
            for(line= line +1; ; line= line + next)
            {
                windices.push_back(0); 
                
                // analyse les attributs du sommet : p/t/n ou p//n ou p/t ou p...
                next= 0;
                int wt= 0;
                int wn= 0;
                if(sscanf(line, " %d/%d/%d %n", &windices.back(), &wt, &wn, &next) == 3) 
                    continue;
                else if(sscanf(line, " %d/%d %n", &windices.back(), &wt, &next) == 2)
                    continue;
                else if(sscanf(line, " %d//%d %n", &windices.back(), &wn, &next) == 2)
                    continue;
                else if(sscanf(line, " %d %n", &windices.back(), &next) == 1)
                    continue;
                else if(next == 0)      // fin de ligne
                    break;
            }
            
            // triangule la face
            for(unsigned v= 2; v +1 < windices.size(); v++)
            {
                unsigned idv[3]= { 0, v -1, v };
                for(unsigned i= 0; i < 3; i++)
                {
                    unsigned k= idv[i];
                    int p= (windices[k] < 0) ? int(wpositions.size()) + windices[k] : windices[k] -1;
                    if(p < 0) break; // error
                    
                    // et duplique les positions...
                    assert(p < int(wpositions.size()));
                    positions.push_back(wpositions[p]);
                }
            }
        }
    }
    
    fclose(in);
    
    if(error)
        printf("[error] loading mesh '%s'...\n%s\n\n", filename, line_buffer);
    else
        printf("mesh '%s': %d positions\n", filename, int(positions.size()));

    return !error;
}


bool read_indexed_positions( const char *filename, std::vector<Point>& positions, std::vector<int>& indices )
{
    positions.clear();
    indices.clear();
    
    FILE *in= fopen(filename, "rt");
    if(!in)
    {
        printf("[error] loading indexed mesh '%s'...\n", filename);
        return false;
    }
    
    printf("loading indexed mesh '%s'...\n", filename);
    
    std::vector<int> windices;
    
    char line_buffer[1024];
    bool error= true;
    for(;;)
    {
        // charge une ligne du fichier
        if(!fgets(line_buffer, sizeof(line_buffer), in))
        {
            error= false;       // fin du fichier, pas d'erreur detectee
            break;
        }
        
        // force la fin de la ligne, au cas ou
        line_buffer[sizeof(line_buffer) -1]= 0;
        
        // saute les espaces en debut de ligne
        char *line= line_buffer;
        while(*line && isspace(*line))
            line++;
        
        if(line[0] == 'v')
        {
            float x, y, z;
            if(line[1] == ' ')          // position x y z
            {
                if(sscanf(line, "v %f %f %f", &x, &y, &z) != 3)
                    break;
                positions.push_back( Point(x, y, z) );
            }
        }
        
        else if(line[0] == 'f')         // triangle a b c, les sommets sont numerotes a partir de 1 ou de la fin du tableau (< 0)
        {
            windices.clear();
            
            int next= 0;
            for(line= line +1; ; line= line + next)
            {
                windices.push_back(0); 
                
                // analyse les attributs du sommet : p/t/n ou p//n ou p/t ou p...
                next= 0;
                int wt= 0;
                int wn= 0;
                if(sscanf(line, " %d/%d/%d %n", &windices.back(), &wt, &wn, &next) == 3) 
                    continue;
                else if(sscanf(line, " %d/%d %n", &windices.back(), &wt, &next) == 2)
                    continue;
                else if(sscanf(line, " %d//%d %n", &windices.back(), &wn, &next) == 2)
                    continue;
                else if(sscanf(line, " %d %n", &windices.back(), &next) == 1)
                    continue;
                else if(next == 0)      // fin de ligne
                    break;
            }
            
            // triangule la face
            for(unsigned v= 2; v +1 < windices.size(); v++)
            {
                unsigned idv[3]= { 0, v -1, v };
                for(unsigned i= 0; i < 3; i++)
                {
                    unsigned k= idv[i];
                    int p= (windices[k] < 0) ? int(positions.size()) + windices[k] : windices[k] -1;
                    if(p < 0) break; // error
                    
                    assert(p < int(positions.size()));
                    indices.push_back(p);
                }
            }
        }
    }
    
    fclose(in);
    
    if(error)
        printf("[error] loading indexed mesh '%s'...\n%s\n\n", filename, line_buffer);
    else
        printf("indexed mesh '%s': %d positions, %d indices\n", filename, int(positions.size()), int(indices.size()));

    return !error;
}


bool read_materials_mtl( const char *filename, Materials& materials  )
{
    FILE *in= fopen(filename, "rt");
    if(!in)
    {
        printf("[error] loading materials '%s'...\n", filename);
        return false;
    }
    
    printf("loading materials '%s'...\n", filename);
    
    Material *material= nullptr;
    char tmp[1024];
    char line_buffer[1024];
    bool error= true;
    for(;;)
    {
        // charge une ligne du fichier
        if(!fgets(line_buffer, sizeof(line_buffer), in))
        {
            error= false;       // fin du fichier, pas d'erreur detectee
            break;
        }
        
        // force la fin de la ligne, au cas ou
        line_buffer[sizeof(line_buffer) -1]= 0;
        
        // saute les espaces en debut de ligne
        char *line= line_buffer;
        while(*line && isspace(*line))
            line++;
        
        if(line[0] == 'n')
        {
            if(sscanf(line, "newmtl %[^\r\n]", tmp) == 1)
            {
                int id= materials.insert(Material(Black()), tmp);
                material= &materials.material(id);
            }
        }
        
        if(material == nullptr)
            continue;
        
        if(line[0] == 'K')
        {
            float r, g, b;
            if(sscanf(line, "Kd %f %f %f", &r, &g, &b) == 3)
                material->diffuse= Color(r, g, b);
            else if(sscanf(line, "Ks %f %f %f", &r, &g, &b) == 3)
                material->specular= Color(r, g, b);
            else if(sscanf(line, "Ke %f %f %f", &r, &g, &b) == 3)
                material->emission= Color(r, g, b);
        }
        
        else if(line[0] == 'N')
        {
            float n;
            if(sscanf(line, "Ns %f", &n) == 1)          // Ns, puissance / concentration du reflet, modele blinn phong
                material->ns= n;
            if(sscanf(line, "Ni %f", &n) == 1)          // Ni, indice de refraction / fresnel
                material->ni= n;
        }
        else if(line[0] == 'T')
        {
            float r, g, b;
            if(sscanf(line, "Tf %f %f %f", &r, &g, &b) == 3)          // Tf, couleur de l'objet transparent
                material->transmission= Color(r, g, b);
        }
        
        else if(line[0] == 'm')
        {
            if(sscanf(line, "map_Kd %[^\r\n]", tmp) == 1)
                material->diffuse_texture= materials.insert_texture( absolute_filename(pathname(filename), tmp).c_str() );
                
            else if(sscanf(line, "map_Ks %[^\r\n]", tmp) == 1)
                material->specular_texture= materials.insert_texture( absolute_filename(pathname(filename), tmp).c_str() );
                
            else if(sscanf(line, "map_Ns %[^\r\n]", tmp) == 1)
                material->ns_texture= materials.insert_texture( absolute_filename(pathname(filename), tmp).c_str() );
        }
    }
    
    fclose(in);
    
    if(error)
        printf("[error] parsing line :\n%s\n", line_buffer);
    
    return !error;
}

bool read_materials( const char *filename, Materials& materials, std::vector<int>& indices )
{
    indices.clear();
    
    FILE *in= fopen(filename, "rt");
    if(!in)
    {
        printf("[error] loading materials '%s'...\n", filename);
        return false;
    }
    
    printf("loading materials '%s'...\n", filename);
    
    std::vector<int> windices;
    int material_id= -1;
    
    char tmp[1024];
    char line_buffer[1024];
    bool error= true;
    for(;;)
    {
        // charge une ligne du fichier
        if(!fgets(line_buffer, sizeof(line_buffer), in))
        {
            error= false;       // fin du fichier, pas d'erreur detectee
            break;
        }
        
        // force la fin de la ligne, au cas ou
        line_buffer[sizeof(line_buffer) -1]= 0;
        
        // saute les espaces en debut de ligne
        char *line= line_buffer;
        while(*line && isspace(*line))
            line++;
        
        if(line[0] == 'f')         // triangle a b c
        {
            windices.clear();
            
            int next= 0;
            for(line= line +1; ; line= line + next)
            {
                windices.push_back(0); 
                
                // analyse les attributs du sommet : p/t/n ou p//n ou p/t ou p...
                next= 0;
                int wt= 0;
                int wn= 0;
                if(sscanf(line, " %d/%d/%d %n", &windices.back(), &wt, &wn, &next) == 3) 
                    continue;
                else if(sscanf(line, " %d/%d %n", &windices.back(), &wt, &next) == 2)
                    continue;
                else if(sscanf(line, " %d//%d %n", &windices.back(), &wn, &next) == 2)
                    continue;
                else if(sscanf(line, " %d %n", &windices.back(), &next) == 1)
                    continue;
                else if(next == 0)      // fin de ligne
                    break;
            }
            
            // triangule la face
            for(unsigned v= 2; v +1 < windices.size(); v++)
                // indice de la matiere de chaque triangle
                indices.push_back(material_id);
        }
        else if(line[0] == 'm')
        {
            if(sscanf(line, "mtllib %[^\r\n]", tmp) == 1)
            {
                std::string materials_filename;
                if(tmp[0] != '/' && tmp[1] != ':')   // windows c:\ pour les chemins complets...
                    materials_filename= normalize_filename(pathname(filename) + tmp);
                else
                    materials_filename= std::string(tmp);
                
                // charge les matieres
                if(!read_materials_mtl( materials_filename.c_str(), materials ))
                    break;
            }
        }
        
        else if(line[0] == 'u')
        {
           if(sscanf(line, "usemtl %[^\r\n]", tmp) == 1)
               material_id= materials.find(tmp);
        }
    }
    
    fclose(in);
    
    if(error)
        printf("[error] loading materials '%s'...\n%s\n\n", filename, line_buffer);

    return !error;
}


bool read_images( const Materials& materials, std::vector<Image>& images )
{
    int n= materials.filename_count();
    if(n == 0)  // pas de textures
        return true;
    
    images.resize(n);
    for(int i= 0; i < n; i++)
    {
        printf("loading '%s'...\n", materials.filename(i));
        images[i]= read_image(materials.filename(i));
    }
    
    return true;
}





