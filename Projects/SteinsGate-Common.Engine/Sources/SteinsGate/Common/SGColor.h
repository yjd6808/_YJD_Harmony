/*
 * 작성자 : 윤정도
 */

#pragma once

#include "Engine.h"

class ColorUtil
{
public:
	static SGColor3B Parse3B(Int32U hex) {
		GLubyte r = (hex >> 16) & 0xff;
		GLubyte g = (hex >>  8) & 0xff;
		GLubyte b = (hex >>  0) & 0xff;
		return { r, g, b };
	}

	static SGColor4B To4B(const SGColor3B& _3bColor) {
		return { _3bColor.r, _3bColor.g, _3bColor.b, 255 };
	}

	static SGColor3B To3B(const SGColor4B& _4bColor) {
		return { _4bColor.r, _4bColor.g, _4bColor.b  };
	}

	static Int32U ToIn32U(const SGColor3B& _3bColor) {
		Int32U color;
		color |= _3bColor.r << 16;
		color |= _3bColor.g << 8;
		color |= _3bColor.b << 0;
		return color;
	}

	static SGColor3B To3B(const Int32U _int32uColor) {
		return SGColor3B((_int32uColor >> 16) & 0xff,
						(_int32uColor >> 8) & 0xff,
						(_int32uColor >> 0) & 0xff );
	}

};

// 색상 표 : https://en.wikipedia.org/wiki/Lists_of_colors 링크 들어가서 A-F 에있는거 뽑음
// 작성한 파싱 소스코드는 맨아래에 붙여놓음 C# 6.0
struct ColorList
{
	static SGColor3B AbsoluteZero_v ;       
	static SGColor3B Acidgreen_v ;          
	static SGColor3B Aero_v ;               
	static SGColor3B Aeroblue_v ;           
	static SGColor3B Africanviolet_v ;      
	static SGColor3B Airsuperiorityblue_v ; 
	static SGColor3B Alabaster_v ;          
	static SGColor3B Alizarin_v ;           
	static SGColor3B Aliceblue_v ;          
	static SGColor3B Alloyorange_v ;        
	static SGColor3B Almond_v ;             
	static SGColor3B Amaranth_v ;           
	static SGColor3B AmaranthMampP_v ;      
	static SGColor3B Amaranthpink_v ;       
	static SGColor3B Amaranthpurple_v ;     
	static SGColor3B Amaranthred_v ;        
	static SGColor3B Amazon_v ;             
	static SGColor3B Amber_v ;              
	static SGColor3B AmberSAEECE_v ;        
	static SGColor3B Amethyst_v ;           
	static SGColor3B Androidgreen_v ;       
	static SGColor3B Antiquebrass_v ;       
	static SGColor3B Antiquebronze_v ;      
	static SGColor3B Antiquefuchsia_v ;     
	static SGColor3B Antiqueruby_v ;        
	static SGColor3B Antiquewhite_v ;       
	static SGColor3B AoEnglish_v ;          
	static SGColor3B Applegreen_v ;         
	static SGColor3B Apricot_v ;            
	static SGColor3B Aqua_v ;               
	static SGColor3B Aquamarine_v ;         
	static SGColor3B Arcticlime_v ;         
	static SGColor3B Armygreen_v ;          
	static SGColor3B Artichoke_v ;          
	static SGColor3B Arylideyellow_v ;      
	static SGColor3B Ashgray_v ;            
	static SGColor3B Asparagus_v ;          
	static SGColor3B Astronaut_v ;          
	static SGColor3B Atomictangerine_v ;    
	static SGColor3B Auburn_v ;             
	static SGColor3B Aureolin_v ;           
	static SGColor3B Avocado_v ;            
	static SGColor3B Azure_v ;              
	static SGColor3B AzureXwebcolor_v ;     
	static SGColor3B Babyblue_v ;           
	static SGColor3B Babyblueeyes_v ;       
	static SGColor3B Babypink_v ;           
	static SGColor3B Babypowder_v ;         
	static SGColor3B BakerMillerpink_v ;    
	static SGColor3B BananaMania_v ;        
	static SGColor3B BarbiePink_v ;         
	static SGColor3B Barnred_v ;            
	static SGColor3B Battleshipgrey_v ;     
	static SGColor3B Beaublue_v ;           
	static SGColor3B Beaver_v ;             
	static SGColor3B Beige_v ;              
	static SGColor3B Bdazzledblue_v ;       
	static SGColor3B Bigdiporuby_v ;        
	static SGColor3B Bisque_v ;             
	static SGColor3B Bistre_v ;             
	static SGColor3B Bistrebrown_v ;        
	static SGColor3B Bitterlemon_v ;        
	static SGColor3B Bitterlime_v ;         
	static SGColor3B Bittersweet_v ;        
	static SGColor3B Bittersweetshimmer_v ; 
	static SGColor3B Black_v ;              
	static SGColor3B Blackbean_v ;          
	static SGColor3B Blackchocolate_v ;     
	static SGColor3B Blackcoffee_v ;        
	static SGColor3B Blackcoral_v ;         
	static SGColor3B Blackolive_v ;         
	static SGColor3B BlackShadows_v ;       
	static SGColor3B Blanchedalmond_v ;     
	static SGColor3B Blastoffbronze_v ;     
	static SGColor3B BleudeFrance_v ;       
	static SGColor3B Blizzardblue_v ;       
	static SGColor3B Blond_v ;              
	static SGColor3B Bloodred_v ;           
	static SGColor3B Blue_v ;               
	static SGColor3B BlueCrayola_v ;        
	static SGColor3B BlueMunsell_v ;        
	static SGColor3B BlueNCS_v ;            
	static SGColor3B BluePantone_v ;        
	static SGColor3B Bluepigment_v ;        
	static SGColor3B BlueRYB_v ;            
	static SGColor3B Bluebell_v ;           
	static SGColor3B Bluegray_v ;           
	static SGColor3B Bluegreen_v ;          
	static SGColor3B Bluegreencolorwheel_v ;
	static SGColor3B Bluejeans_v ;          
	static SGColor3B Bluesapphire_v ;       
	static SGColor3B Blueviolet_v ;         
	static SGColor3B BluevioletCrayola_v ;  
	static SGColor3B Bluevioletcolorwheel_v;
	static SGColor3B Blueyonder_v ;         
	static SGColor3B Bluetiful_v ;          
	static SGColor3B Blush_v ;              
	static SGColor3B Bole_v ;               
	static SGColor3B Bone_v ;               
	static SGColor3B Bottlegreen_v ;        
	static SGColor3B Brandy_v ;             
	static SGColor3B Brickred_v ;           
	static SGColor3B Brightgreen_v ;        
	static SGColor3B Brightlilac_v ;        
	static SGColor3B Brightmaroon_v ;       
	static SGColor3B Brightnavyblue_v ;     
	static SGColor3B BrightyellowCrayola_v ;
	static SGColor3B Brilliantrose_v ;      
	static SGColor3B Brinkpink_v ;          
	static SGColor3B Britishracinggreen_v ; 
	static SGColor3B Bronze_v ;             
	static SGColor3B Brown_v ;              
	static SGColor3B Brownsugar_v ;         
	static SGColor3B Brunswickgreen_v ;     
	static SGColor3B Budgreen_v ;           
	static SGColor3B Buff_v ;               
	static SGColor3B Burgundy_v ;           
	static SGColor3B Burlywood_v ;          
	static SGColor3B Burnishedbrown_v ;     
	static SGColor3B Burntorange_v ;        
	static SGColor3B Burntsienna_v ;        
	static SGColor3B Burntumber_v ;         
	static SGColor3B Byzantine_v ;          
	static SGColor3B Byzantium_v ;          
	static SGColor3B Cadet_v ;              
	static SGColor3B Cadetblue_v ;          
	static SGColor3B CadetblueCrayola_v ;   
	static SGColor3B Cadetgrey_v ;          
	static SGColor3B Cadmiumgreen_v ;       
	static SGColor3B Cadmiumorange_v ;      
	static SGColor3B Cadmiumred_v ;         
	static SGColor3B Cadmiumyellow_v ;      
	static SGColor3B Cafaulait_v ;          
	static SGColor3B Cafnoir_v ;            
	static SGColor3B Cambridgeblue_v ;      
	static SGColor3B Camel_v ;              
	static SGColor3B Cameopink_v ;          
	static SGColor3B Canary_v ;             
	static SGColor3B Canaryyellow_v ;       
	static SGColor3B Candyapplered_v ;      
	static SGColor3B Candypink_v ;          
	static SGColor3B Capri_v ;              
	static SGColor3B Caputmortuum_v ;       
	static SGColor3B Cardinal_v ;           
	static SGColor3B Caribbeangreen_v ;     
	static SGColor3B Carmine_v ;            
	static SGColor3B CarmineMampP_v ;       
	static SGColor3B Carnationpink_v ;      
	static SGColor3B Carnelian_v ;          
	static SGColor3B Carolinablue_v ;       
	static SGColor3B Carrotorange_v ;       
	static SGColor3B Castletongreen_v ;     
	static SGColor3B Catawba_v ;            
	static SGColor3B CedarChest_v ;         
	static SGColor3B Celadon_v ;            
	static SGColor3B Celadonblue_v ;        
	static SGColor3B Celadongreen_v ;       
	static SGColor3B Celeste_v ;            
	static SGColor3B Celticblue_v ;         
	static SGColor3B Cerise_v ;             
	static SGColor3B Cerulean_v ;           
	static SGColor3B Ceruleanblue_v ;       
	static SGColor3B Ceruleanfrost_v ;      
	static SGColor3B CeruleanCrayola_v ;    
	static SGColor3B CGblue_v ;             
	static SGColor3B CGred_v ;              
	static SGColor3B Champagne_v ;          
	static SGColor3B Champagnepink_v ;      
	static SGColor3B Charcoal_v ;           
	static SGColor3B Charlestongreen_v ;    
	static SGColor3B Charmpink_v ;          
	static SGColor3B Chartreusetraditional_v;
	static SGColor3B Chartreuseweb_v ;      
	static SGColor3B Cherryblossompink_v ;  
	static SGColor3B Chestnut_v ;           
	static SGColor3B Chilired_v ;           
	static SGColor3B Chinapink_v ;          
	static SGColor3B Chinarose_v ;          
	static SGColor3B Chinesered_v ;         
	static SGColor3B Chineseviolet_v ;      
	static SGColor3B Chineseyellow_v ;      
	static SGColor3B Chocolatetraditional_v;
	static SGColor3B Chocolateweb_v ;       
	static SGColor3B ChocolateCosmos_v ;    
	static SGColor3B Chromeyellow_v ;       
	static SGColor3B Cinereous_v ;          
	static SGColor3B Cinnabar_v ;           
	static SGColor3B CinnamonSatin_v ;      
	static SGColor3B Citrine_v ;            
	static SGColor3B Citron_v ;             
	static SGColor3B Claret_v ;             
	static SGColor3B Cobaltblue_v ;         
	static SGColor3B Cocoabrown_v ;         
	static SGColor3B Coffee_v ;             
	static SGColor3B ColumbiaBlue_v ;       
	static SGColor3B Congopink_v ;          
	static SGColor3B Coolgrey_v ;           
	static SGColor3B Copper_v ;             
	static SGColor3B CopperCrayola_v ;      
	static SGColor3B Copperpenny_v ;        
	static SGColor3B Copperred_v ;          
	static SGColor3B Copperrose_v ;         
	static SGColor3B Coquelicot_v ;         
	static SGColor3B Coral_v ;              
	static SGColor3B Coralpink_v ;          
	static SGColor3B Cordovan_v ;           
	static SGColor3B Corn_v ;               
	static SGColor3B Cornellred_v ;         
	static SGColor3B Cornflowerblue_v ;     
	static SGColor3B Cornsilk_v ;           
	static SGColor3B Cosmiccobalt_v ;       
	static SGColor3B Cosmiclatte_v ;        
	static SGColor3B Coyotebrown_v ;        
	static SGColor3B Cottoncandy_v ;        
	static SGColor3B Cream_v ;              
	static SGColor3B Crimson_v ;            
	static SGColor3B CrimsonUA_v ;          
	static SGColor3B Crystal_v ;            
	static SGColor3B Cultured_v ;           
	static SGColor3B Cyan_v ;               
	static SGColor3B Cyanprocess_v ;        
	static SGColor3B Cybergrape_v ;         
	static SGColor3B Cyberyellow_v ;        
	static SGColor3B Cyclamen_v ;           
	static SGColor3B Darkbluegray_v ;       
	static SGColor3B Darkbrown_v ;          
	static SGColor3B Darkbyzantium_v ;      
	static SGColor3B Darkcornflowerblue_v ; 
	static SGColor3B Darkcyan_v ;           
	static SGColor3B Darkelectricblue_v ;   
	static SGColor3B Darkgoldenrod_v ;      
	static SGColor3B Darkgreen_v ;          
	static SGColor3B DarkgreenX_v ;         
	static SGColor3B Darkjunglegreen_v ;    
	static SGColor3B Darkkhaki_v ;          
	static SGColor3B Darklava_v ;           
	static SGColor3B Darkliver_v ;          
	static SGColor3B Darkliverhorses_v ;    
	static SGColor3B Darkmagenta_v ;        
	static SGColor3B Darkmossgreen_v ;      
	static SGColor3B Darkolivegreen_v ;     
	static SGColor3B Darkorange_v ;         
	static SGColor3B Darkorchid_v ;         
	static SGColor3B Darkpastelgreen_v ;    
	static SGColor3B Darkpurple_v ;         
	static SGColor3B Darkred_v ;            
	static SGColor3B Darksalmon_v ;         
	static SGColor3B Darkseagreen_v ;       
	static SGColor3B Darksienna_v ;         
	static SGColor3B Darkskyblue_v ;        
	static SGColor3B Darkslateblue_v ;      
	static SGColor3B Darkslategray_v ;      
	static SGColor3B Darkspringgreen_v ;    
	static SGColor3B Darkturquoise_v ;      
	static SGColor3B Darkviolet_v ;         
	static SGColor3B Dartmouthgreen_v ;     
	static SGColor3B Davysgrey_v ;          
	static SGColor3B Deepcerise_v ;         
	static SGColor3B Deepchampagne_v ;      
	static SGColor3B Deepchestnut_v ;       
	static SGColor3B Deepjunglegreen_v ;    
	static SGColor3B Deeppink_v ;           
	static SGColor3B Deepsaffron_v ;        
	static SGColor3B Deepskyblue_v ;        
	static SGColor3B DeepSpaceSparkle_v ;   
	static SGColor3B Deeptaupe_v ;          
	static SGColor3B Denim_v ;              
	static SGColor3B Denimblue_v ;          
	static SGColor3B Desert_v ;             
	static SGColor3B Desertsand_v ;         
	static SGColor3B Dimgray_v ;            
	static SGColor3B Dodgerblue_v ;         
	static SGColor3B Dogwoodrose_v ;        
	static SGColor3B Drab_v ;               
	static SGColor3B Dukeblue_v ;           
	static SGColor3B Dutchwhite_v ;         
	static SGColor3B Earthyellow_v ;        
	static SGColor3B Ebony_v ;              
	static SGColor3B Ecru_v ;               
	static SGColor3B Eerieblack_v ;         
	static SGColor3B Eggplant_v ;           
	static SGColor3B Eggshell_v ;           
	static SGColor3B Egyptianblue_v ;       
	static SGColor3B Eigengrau_v ;          
	static SGColor3B Electricblue_v ;       
	static SGColor3B Electricgreen_v ;      
	static SGColor3B Electricindigo_v ;     
	static SGColor3B Electriclime_v ;       
	static SGColor3B Electricpurple_v ;     
	static SGColor3B Electricviolet_v ;     
	static SGColor3B Emerald_v ;            
	static SGColor3B Eminence_v ;           
	static SGColor3B Englishgreen_v ;       
	static SGColor3B Englishlavender_v ;    
	static SGColor3B Englishred_v ;         
	static SGColor3B Englishvermillion_v ;  
	static SGColor3B Englishviolet_v ;      
	static SGColor3B Erin_v ;               
	static SGColor3B Etonblue_v ;           
	static SGColor3B Fallow_v ;             
	static SGColor3B Falured_v ;            
	static SGColor3B Fandango_v ;           
	static SGColor3B Fandangopink_v ;       
	static SGColor3B Fashionfuchsia_v ;     
	static SGColor3B Fawn_v ;               
	static SGColor3B Feldgrau_v ;           
	static SGColor3B Ferngreen_v ;          
	static SGColor3B Fielddrab_v ;          
	static SGColor3B Fieryrose_v ;          
	static SGColor3B Firebrick_v ;          
	static SGColor3B Fireenginered_v ;      
	static SGColor3B Fireopal_v ;           
	static SGColor3B Flame_v ;              
	static SGColor3B Flax_v ;               
	static SGColor3B Flirt_v ;              
	static SGColor3B Floralwhite_v ;        
	static SGColor3B Fluorescentblue_v ;    
	static SGColor3B ForestgreenCrayola_v ; 
	static SGColor3B Forestgreentraditional_v;
	static SGColor3B Forestgreenweb_v ;     
	static SGColor3B Frenchbeige_v ;        
	static SGColor3B Frenchbistre_v ;       
	static SGColor3B Frenchblue_v ;         
	static SGColor3B Frenchfuchsia_v ;      
	static SGColor3B Frenchlilac_v ;        
	static SGColor3B Frenchlime_v ;         
	static SGColor3B Frenchmauve_v ;        
	static SGColor3B Frenchpink_v ;         
	static SGColor3B Frenchraspberry_v ;    
	static SGColor3B Frenchrose_v ;         
	static SGColor3B Frenchskyblue_v ;      
	static SGColor3B Frenchviolet_v ;       
	static SGColor3B Frostbite_v ;          
	static SGColor3B Fuchsia_v ;            
	static SGColor3B FuchsiaCrayola_v ;     
	static SGColor3B Fuchsiapurple_v ;      
	static SGColor3B Fuchsiarose_v ;        
	static SGColor3B Fulvous_v ;            
	static SGColor3B FuzzyWuzzy_v ;         
};



/*
파싱 소스코드 (.net6.0 바로 복붙해서 빌드하면댐)

using System.Net;

Console.WriteLine("Hello, World!");

WebClient client ; new WebClient();
string result ; client.DownloadString("https://en.wikipedia.org/wiki/List_of_colors:_A%E2%80%93F");

List<string> colorlist ; new List<string>();
List<string> hexlist ; new List<string>();

result.Split("<td style;\"border-left:solid 2px #AAA;border-right:solid 1px #AAA;font-family:monospace;\">").ToList().ForEach(x ;>
{
	string last20 ; string.Empty;

	for (int i ; x.Length - 41; i < x.Length; i++)
	{
		last20 +; x[i];
	}

	try
	{
		colorlist.Add(last20.Split("</a>")[0].Split(">")[1]);
	}
	catch (Exception ex)
	{

	}

});

result.Split("<td style;\"border-left:solid 2px #AAA;border-right:solid 1px #AAA;font-family:monospace;\">")
	.ToList().Skip(1).ToList().ForEach((x) ;>
	{
		string guessedHex ; x.Substring(0, 7);
		if (guessedHex.StartsWith("#"))
			hexlist.Add(guessedHex);
	});

// static cocos2d::SGColor3B ColorAbsoluteZero_v ; { 0x00, 0x48, 0xBA };
for (int i ;0; i < hexlist.Count; i++)
{

	string s ; colorlist[i];
	string copy ; "static cocos2d::SGColor3B ";
	for (int j ;0;j < s.Length; j++)
	{
		if ((s[j] >; 'a' && s[j] <;'z') || (s[j] >; 'A' && s[j] <; 'Z'))
			copy +; s[j];


	}
	copy +; "_v ; ";

	hexlist[i] ; hexlist[i].Replace("#", "");

	copy +; "\t\t\t{ 0x" +  hexlist[i].Substring(0, 2) + ", 0x" + hexlist[i].Substring(2, 2) + ", 0x" + hexlist[i].Substring(4, 2) + " };";

	Console.WriteLine(copy);
}


*/