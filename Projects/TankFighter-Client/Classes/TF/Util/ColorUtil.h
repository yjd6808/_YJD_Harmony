/*
 * 작성자 : 윤정도
 */

#pragma once

#include <JCore/Type.h>
#include <cocos2d.h>


class ColorUtil
{
public:
	static cocos2d::Color3B Parse3B(Int32U hex) {
		GLubyte r = (hex >> 16) & 0xff;
		GLubyte g = (hex >>  8) & 0xff;
		GLubyte b = (hex >>  0) & 0xff;
		return { r, g, b };
	}
	static cocos2d::Color4B To4B(const cocos2d::Color3B& _3bColor) {
		return { _3bColor.r, _3bColor.g, _3bColor.b, 255 };
	}

	static cocos2d::Color3B To3B(const cocos2d::Color4B& _4bColor) {
		return { _4bColor.r, _4bColor.g, _4bColor.b  };
	}

	static Int32U ToIn32U(const cocos2d::Color3B& _3bColor) {
		Int32U color;
		color |= _3bColor.r << 16;
		color |= _3bColor.g << 8;
		color |= _3bColor.b << 0;
		return color;
	}

	static cocos2d::Color3B To3B(const Int32U _int32uColor) {
		return cocos2d::Color3B((_int32uColor >> 16) & 0xff,
								(_int32uColor >> 8) & 0xff,
								(_int32uColor >> 0) & 0xff );
	}

};

// 색상 표 : https://en.wikipedia.org/wiki/Lists_of_colors 링크 들어가서 A-F 에있는거 뽑음
// 작성한 파싱 소스코드는 맨아래에 붙여놓음 C# 6.0
struct ColorList
{
	static cocos2d::Color3B AbsoluteZero_v ;       
	static cocos2d::Color3B Acidgreen_v ;          
	static cocos2d::Color3B Aero_v ;               
	static cocos2d::Color3B Aeroblue_v ;           
	static cocos2d::Color3B Africanviolet_v ;      
	static cocos2d::Color3B Airsuperiorityblue_v ; 
	static cocos2d::Color3B Alabaster_v ;          
	static cocos2d::Color3B Alizarin_v ;           
	static cocos2d::Color3B Aliceblue_v ;          
	static cocos2d::Color3B Alloyorange_v ;        
	static cocos2d::Color3B Almond_v ;             
	static cocos2d::Color3B Amaranth_v ;           
	static cocos2d::Color3B AmaranthMampP_v ;      
	static cocos2d::Color3B Amaranthpink_v ;       
	static cocos2d::Color3B Amaranthpurple_v ;     
	static cocos2d::Color3B Amaranthred_v ;        
	static cocos2d::Color3B Amazon_v ;             
	static cocos2d::Color3B Amber_v ;              
	static cocos2d::Color3B AmberSAEECE_v ;        
	static cocos2d::Color3B Amethyst_v ;           
	static cocos2d::Color3B Androidgreen_v ;       
	static cocos2d::Color3B Antiquebrass_v ;       
	static cocos2d::Color3B Antiquebronze_v ;      
	static cocos2d::Color3B Antiquefuchsia_v ;     
	static cocos2d::Color3B Antiqueruby_v ;        
	static cocos2d::Color3B Antiquewhite_v ;       
	static cocos2d::Color3B AoEnglish_v ;          
	static cocos2d::Color3B Applegreen_v ;         
	static cocos2d::Color3B Apricot_v ;            
	static cocos2d::Color3B Aqua_v ;               
	static cocos2d::Color3B Aquamarine_v ;         
	static cocos2d::Color3B Arcticlime_v ;         
	static cocos2d::Color3B Armygreen_v ;          
	static cocos2d::Color3B Artichoke_v ;          
	static cocos2d::Color3B Arylideyellow_v ;      
	static cocos2d::Color3B Ashgray_v ;            
	static cocos2d::Color3B Asparagus_v ;          
	static cocos2d::Color3B Astronaut_v ;          
	static cocos2d::Color3B Atomictangerine_v ;    
	static cocos2d::Color3B Auburn_v ;             
	static cocos2d::Color3B Aureolin_v ;           
	static cocos2d::Color3B Avocado_v ;            
	static cocos2d::Color3B Azure_v ;              
	static cocos2d::Color3B AzureXwebcolor_v ;     
	static cocos2d::Color3B Babyblue_v ;           
	static cocos2d::Color3B Babyblueeyes_v ;       
	static cocos2d::Color3B Babypink_v ;           
	static cocos2d::Color3B Babypowder_v ;         
	static cocos2d::Color3B BakerMillerpink_v ;    
	static cocos2d::Color3B BananaMania_v ;        
	static cocos2d::Color3B BarbiePink_v ;         
	static cocos2d::Color3B Barnred_v ;            
	static cocos2d::Color3B Battleshipgrey_v ;     
	static cocos2d::Color3B Beaublue_v ;           
	static cocos2d::Color3B Beaver_v ;             
	static cocos2d::Color3B Beige_v ;              
	static cocos2d::Color3B Bdazzledblue_v ;       
	static cocos2d::Color3B Bigdiporuby_v ;        
	static cocos2d::Color3B Bisque_v ;             
	static cocos2d::Color3B Bistre_v ;             
	static cocos2d::Color3B Bistrebrown_v ;        
	static cocos2d::Color3B Bitterlemon_v ;        
	static cocos2d::Color3B Bitterlime_v ;         
	static cocos2d::Color3B Bittersweet_v ;        
	static cocos2d::Color3B Bittersweetshimmer_v ; 
	static cocos2d::Color3B Black_v ;              
	static cocos2d::Color3B Blackbean_v ;          
	static cocos2d::Color3B Blackchocolate_v ;     
	static cocos2d::Color3B Blackcoffee_v ;        
	static cocos2d::Color3B Blackcoral_v ;         
	static cocos2d::Color3B Blackolive_v ;         
	static cocos2d::Color3B BlackShadows_v ;       
	static cocos2d::Color3B Blanchedalmond_v ;     
	static cocos2d::Color3B Blastoffbronze_v ;     
	static cocos2d::Color3B BleudeFrance_v ;       
	static cocos2d::Color3B Blizzardblue_v ;       
	static cocos2d::Color3B Blond_v ;              
	static cocos2d::Color3B Bloodred_v ;           
	static cocos2d::Color3B Blue_v ;               
	static cocos2d::Color3B BlueCrayola_v ;        
	static cocos2d::Color3B BlueMunsell_v ;        
	static cocos2d::Color3B BlueNCS_v ;            
	static cocos2d::Color3B BluePantone_v ;        
	static cocos2d::Color3B Bluepigment_v ;        
	static cocos2d::Color3B BlueRYB_v ;            
	static cocos2d::Color3B Bluebell_v ;           
	static cocos2d::Color3B Bluegray_v ;           
	static cocos2d::Color3B Bluegreen_v ;          
	static cocos2d::Color3B Bluegreencolorwheel_v ;
	static cocos2d::Color3B Bluejeans_v ;          
	static cocos2d::Color3B Bluesapphire_v ;       
	static cocos2d::Color3B Blueviolet_v ;         
	static cocos2d::Color3B BluevioletCrayola_v ;  
	static cocos2d::Color3B Bluevioletcolorwheel_v;
	static cocos2d::Color3B Blueyonder_v ;         
	static cocos2d::Color3B Bluetiful_v ;          
	static cocos2d::Color3B Blush_v ;              
	static cocos2d::Color3B Bole_v ;               
	static cocos2d::Color3B Bone_v ;               
	static cocos2d::Color3B Bottlegreen_v ;        
	static cocos2d::Color3B Brandy_v ;             
	static cocos2d::Color3B Brickred_v ;           
	static cocos2d::Color3B Brightgreen_v ;        
	static cocos2d::Color3B Brightlilac_v ;        
	static cocos2d::Color3B Brightmaroon_v ;       
	static cocos2d::Color3B Brightnavyblue_v ;     
	static cocos2d::Color3B BrightyellowCrayola_v ;
	static cocos2d::Color3B Brilliantrose_v ;      
	static cocos2d::Color3B Brinkpink_v ;          
	static cocos2d::Color3B Britishracinggreen_v ; 
	static cocos2d::Color3B Bronze_v ;             
	static cocos2d::Color3B Brown_v ;              
	static cocos2d::Color3B Brownsugar_v ;         
	static cocos2d::Color3B Brunswickgreen_v ;     
	static cocos2d::Color3B Budgreen_v ;           
	static cocos2d::Color3B Buff_v ;               
	static cocos2d::Color3B Burgundy_v ;           
	static cocos2d::Color3B Burlywood_v ;          
	static cocos2d::Color3B Burnishedbrown_v ;     
	static cocos2d::Color3B Burntorange_v ;        
	static cocos2d::Color3B Burntsienna_v ;        
	static cocos2d::Color3B Burntumber_v ;         
	static cocos2d::Color3B Byzantine_v ;          
	static cocos2d::Color3B Byzantium_v ;          
	static cocos2d::Color3B Cadet_v ;              
	static cocos2d::Color3B Cadetblue_v ;          
	static cocos2d::Color3B CadetblueCrayola_v ;   
	static cocos2d::Color3B Cadetgrey_v ;          
	static cocos2d::Color3B Cadmiumgreen_v ;       
	static cocos2d::Color3B Cadmiumorange_v ;      
	static cocos2d::Color3B Cadmiumred_v ;         
	static cocos2d::Color3B Cadmiumyellow_v ;      
	static cocos2d::Color3B Cafaulait_v ;          
	static cocos2d::Color3B Cafnoir_v ;            
	static cocos2d::Color3B Cambridgeblue_v ;      
	static cocos2d::Color3B Camel_v ;              
	static cocos2d::Color3B Cameopink_v ;          
	static cocos2d::Color3B Canary_v ;             
	static cocos2d::Color3B Canaryyellow_v ;       
	static cocos2d::Color3B Candyapplered_v ;      
	static cocos2d::Color3B Candypink_v ;          
	static cocos2d::Color3B Capri_v ;              
	static cocos2d::Color3B Caputmortuum_v ;       
	static cocos2d::Color3B Cardinal_v ;           
	static cocos2d::Color3B Caribbeangreen_v ;     
	static cocos2d::Color3B Carmine_v ;            
	static cocos2d::Color3B CarmineMampP_v ;       
	static cocos2d::Color3B Carnationpink_v ;      
	static cocos2d::Color3B Carnelian_v ;          
	static cocos2d::Color3B Carolinablue_v ;       
	static cocos2d::Color3B Carrotorange_v ;       
	static cocos2d::Color3B Castletongreen_v ;     
	static cocos2d::Color3B Catawba_v ;            
	static cocos2d::Color3B CedarChest_v ;         
	static cocos2d::Color3B Celadon_v ;            
	static cocos2d::Color3B Celadonblue_v ;        
	static cocos2d::Color3B Celadongreen_v ;       
	static cocos2d::Color3B Celeste_v ;            
	static cocos2d::Color3B Celticblue_v ;         
	static cocos2d::Color3B Cerise_v ;             
	static cocos2d::Color3B Cerulean_v ;           
	static cocos2d::Color3B Ceruleanblue_v ;       
	static cocos2d::Color3B Ceruleanfrost_v ;      
	static cocos2d::Color3B CeruleanCrayola_v ;    
	static cocos2d::Color3B CGblue_v ;             
	static cocos2d::Color3B CGred_v ;              
	static cocos2d::Color3B Champagne_v ;          
	static cocos2d::Color3B Champagnepink_v ;      
	static cocos2d::Color3B Charcoal_v ;           
	static cocos2d::Color3B Charlestongreen_v ;    
	static cocos2d::Color3B Charmpink_v ;          
	static cocos2d::Color3B Chartreusetraditional_v;
	static cocos2d::Color3B Chartreuseweb_v ;      
	static cocos2d::Color3B Cherryblossompink_v ;  
	static cocos2d::Color3B Chestnut_v ;           
	static cocos2d::Color3B Chilired_v ;           
	static cocos2d::Color3B Chinapink_v ;          
	static cocos2d::Color3B Chinarose_v ;          
	static cocos2d::Color3B Chinesered_v ;         
	static cocos2d::Color3B Chineseviolet_v ;      
	static cocos2d::Color3B Chineseyellow_v ;      
	static cocos2d::Color3B Chocolatetraditional_v;
	static cocos2d::Color3B Chocolateweb_v ;       
	static cocos2d::Color3B ChocolateCosmos_v ;    
	static cocos2d::Color3B Chromeyellow_v ;       
	static cocos2d::Color3B Cinereous_v ;          
	static cocos2d::Color3B Cinnabar_v ;           
	static cocos2d::Color3B CinnamonSatin_v ;      
	static cocos2d::Color3B Citrine_v ;            
	static cocos2d::Color3B Citron_v ;             
	static cocos2d::Color3B Claret_v ;             
	static cocos2d::Color3B Cobaltblue_v ;         
	static cocos2d::Color3B Cocoabrown_v ;         
	static cocos2d::Color3B Coffee_v ;             
	static cocos2d::Color3B ColumbiaBlue_v ;       
	static cocos2d::Color3B Congopink_v ;          
	static cocos2d::Color3B Coolgrey_v ;           
	static cocos2d::Color3B Copper_v ;             
	static cocos2d::Color3B CopperCrayola_v ;      
	static cocos2d::Color3B Copperpenny_v ;        
	static cocos2d::Color3B Copperred_v ;          
	static cocos2d::Color3B Copperrose_v ;         
	static cocos2d::Color3B Coquelicot_v ;         
	static cocos2d::Color3B Coral_v ;              
	static cocos2d::Color3B Coralpink_v ;          
	static cocos2d::Color3B Cordovan_v ;           
	static cocos2d::Color3B Corn_v ;               
	static cocos2d::Color3B Cornellred_v ;         
	static cocos2d::Color3B Cornflowerblue_v ;     
	static cocos2d::Color3B Cornsilk_v ;           
	static cocos2d::Color3B Cosmiccobalt_v ;       
	static cocos2d::Color3B Cosmiclatte_v ;        
	static cocos2d::Color3B Coyotebrown_v ;        
	static cocos2d::Color3B Cottoncandy_v ;        
	static cocos2d::Color3B Cream_v ;              
	static cocos2d::Color3B Crimson_v ;            
	static cocos2d::Color3B CrimsonUA_v ;          
	static cocos2d::Color3B Crystal_v ;            
	static cocos2d::Color3B Cultured_v ;           
	static cocos2d::Color3B Cyan_v ;               
	static cocos2d::Color3B Cyanprocess_v ;        
	static cocos2d::Color3B Cybergrape_v ;         
	static cocos2d::Color3B Cyberyellow_v ;        
	static cocos2d::Color3B Cyclamen_v ;           
	static cocos2d::Color3B Darkbluegray_v ;       
	static cocos2d::Color3B Darkbrown_v ;          
	static cocos2d::Color3B Darkbyzantium_v ;      
	static cocos2d::Color3B Darkcornflowerblue_v ; 
	static cocos2d::Color3B Darkcyan_v ;           
	static cocos2d::Color3B Darkelectricblue_v ;   
	static cocos2d::Color3B Darkgoldenrod_v ;      
	static cocos2d::Color3B Darkgreen_v ;          
	static cocos2d::Color3B DarkgreenX_v ;         
	static cocos2d::Color3B Darkjunglegreen_v ;    
	static cocos2d::Color3B Darkkhaki_v ;          
	static cocos2d::Color3B Darklava_v ;           
	static cocos2d::Color3B Darkliver_v ;          
	static cocos2d::Color3B Darkliverhorses_v ;    
	static cocos2d::Color3B Darkmagenta_v ;        
	static cocos2d::Color3B Darkmossgreen_v ;      
	static cocos2d::Color3B Darkolivegreen_v ;     
	static cocos2d::Color3B Darkorange_v ;         
	static cocos2d::Color3B Darkorchid_v ;         
	static cocos2d::Color3B Darkpastelgreen_v ;    
	static cocos2d::Color3B Darkpurple_v ;         
	static cocos2d::Color3B Darkred_v ;            
	static cocos2d::Color3B Darksalmon_v ;         
	static cocos2d::Color3B Darkseagreen_v ;       
	static cocos2d::Color3B Darksienna_v ;         
	static cocos2d::Color3B Darkskyblue_v ;        
	static cocos2d::Color3B Darkslateblue_v ;      
	static cocos2d::Color3B Darkslategray_v ;      
	static cocos2d::Color3B Darkspringgreen_v ;    
	static cocos2d::Color3B Darkturquoise_v ;      
	static cocos2d::Color3B Darkviolet_v ;         
	static cocos2d::Color3B Dartmouthgreen_v ;     
	static cocos2d::Color3B Davysgrey_v ;          
	static cocos2d::Color3B Deepcerise_v ;         
	static cocos2d::Color3B Deepchampagne_v ;      
	static cocos2d::Color3B Deepchestnut_v ;       
	static cocos2d::Color3B Deepjunglegreen_v ;    
	static cocos2d::Color3B Deeppink_v ;           
	static cocos2d::Color3B Deepsaffron_v ;        
	static cocos2d::Color3B Deepskyblue_v ;        
	static cocos2d::Color3B DeepSpaceSparkle_v ;   
	static cocos2d::Color3B Deeptaupe_v ;          
	static cocos2d::Color3B Denim_v ;              
	static cocos2d::Color3B Denimblue_v ;          
	static cocos2d::Color3B Desert_v ;             
	static cocos2d::Color3B Desertsand_v ;         
	static cocos2d::Color3B Dimgray_v ;            
	static cocos2d::Color3B Dodgerblue_v ;         
	static cocos2d::Color3B Dogwoodrose_v ;        
	static cocos2d::Color3B Drab_v ;               
	static cocos2d::Color3B Dukeblue_v ;           
	static cocos2d::Color3B Dutchwhite_v ;         
	static cocos2d::Color3B Earthyellow_v ;        
	static cocos2d::Color3B Ebony_v ;              
	static cocos2d::Color3B Ecru_v ;               
	static cocos2d::Color3B Eerieblack_v ;         
	static cocos2d::Color3B Eggplant_v ;           
	static cocos2d::Color3B Eggshell_v ;           
	static cocos2d::Color3B Egyptianblue_v ;       
	static cocos2d::Color3B Eigengrau_v ;          
	static cocos2d::Color3B Electricblue_v ;       
	static cocos2d::Color3B Electricgreen_v ;      
	static cocos2d::Color3B Electricindigo_v ;     
	static cocos2d::Color3B Electriclime_v ;       
	static cocos2d::Color3B Electricpurple_v ;     
	static cocos2d::Color3B Electricviolet_v ;     
	static cocos2d::Color3B Emerald_v ;            
	static cocos2d::Color3B Eminence_v ;           
	static cocos2d::Color3B Englishgreen_v ;       
	static cocos2d::Color3B Englishlavender_v ;    
	static cocos2d::Color3B Englishred_v ;         
	static cocos2d::Color3B Englishvermillion_v ;  
	static cocos2d::Color3B Englishviolet_v ;      
	static cocos2d::Color3B Erin_v ;               
	static cocos2d::Color3B Etonblue_v ;           
	static cocos2d::Color3B Fallow_v ;             
	static cocos2d::Color3B Falured_v ;            
	static cocos2d::Color3B Fandango_v ;           
	static cocos2d::Color3B Fandangopink_v ;       
	static cocos2d::Color3B Fashionfuchsia_v ;     
	static cocos2d::Color3B Fawn_v ;               
	static cocos2d::Color3B Feldgrau_v ;           
	static cocos2d::Color3B Ferngreen_v ;          
	static cocos2d::Color3B Fielddrab_v ;          
	static cocos2d::Color3B Fieryrose_v ;          
	static cocos2d::Color3B Firebrick_v ;          
	static cocos2d::Color3B Fireenginered_v ;      
	static cocos2d::Color3B Fireopal_v ;           
	static cocos2d::Color3B Flame_v ;              
	static cocos2d::Color3B Flax_v ;               
	static cocos2d::Color3B Flirt_v ;              
	static cocos2d::Color3B Floralwhite_v ;        
	static cocos2d::Color3B Fluorescentblue_v ;    
	static cocos2d::Color3B ForestgreenCrayola_v ; 
	static cocos2d::Color3B Forestgreentraditional_v;
	static cocos2d::Color3B Forestgreenweb_v ;     
	static cocos2d::Color3B Frenchbeige_v ;        
	static cocos2d::Color3B Frenchbistre_v ;       
	static cocos2d::Color3B Frenchblue_v ;         
	static cocos2d::Color3B Frenchfuchsia_v ;      
	static cocos2d::Color3B Frenchlilac_v ;        
	static cocos2d::Color3B Frenchlime_v ;         
	static cocos2d::Color3B Frenchmauve_v ;        
	static cocos2d::Color3B Frenchpink_v ;         
	static cocos2d::Color3B Frenchraspberry_v ;    
	static cocos2d::Color3B Frenchrose_v ;         
	static cocos2d::Color3B Frenchskyblue_v ;      
	static cocos2d::Color3B Frenchviolet_v ;       
	static cocos2d::Color3B Frostbite_v ;          
	static cocos2d::Color3B Fuchsia_v ;            
	static cocos2d::Color3B FuchsiaCrayola_v ;     
	static cocos2d::Color3B Fuchsiapurple_v ;      
	static cocos2d::Color3B Fuchsiarose_v ;        
	static cocos2d::Color3B Fulvous_v ;            
	static cocos2d::Color3B FuzzyWuzzy_v ;         
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

// static cocos2d::cocos2d::Color3B ColorAbsoluteZero_v ; { 0x00, 0x48, 0xBA };
for (int i ;0; i < hexlist.Count; i++)
{

	string s ; colorlist[i];
	string copy ; "static cocos2d::cocos2d::Color3B ";
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