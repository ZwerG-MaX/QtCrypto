/**
 * Copyright (C)  2004  Brad Hards <bradh@frogmouth.net>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "bigintunittest.h"
#include "qca.h"

#define BUILD_WITH_NEGATIVE_CONSTRUCTOR 1
BigIntUnitTest::BigIntUnitTest()
    : Tester()
{
    QCA::Initializer init; // allocates locked memory
}

void BigIntUnitTest::allTests()
{
    QBigInteger result;

    // Some string conversion tests
    CHECK( QBigInteger("255").toString(), QBigInteger(255).toString() );
    CHECK( QBigInteger("-255").toString(), QBigInteger(-255).toString() );
    CHECK( QBigInteger("255").toString(), QString("255") );
    CHECK( QBigInteger("-255").toString(), QString("-255") );

    CHECK( QBigInteger("0377").toString(), QBigInteger(255).toString() );
    CHECK( QBigInteger("-0377").toString(), QBigInteger(255).toString() );
    CHECK( QBigInteger("0377").toString(), QString("255") );
    CHECK( QBigInteger("-0377").toString(), QString("-255") );

    // Botan's addition tests
    result = QBigInteger( 255 ) += QBigInteger( 1 );
    CHECK( result.toString(), QBigInteger( 256 ).toString() );

    result = QBigInteger( "65535" ) += QBigInteger( "1" );
    CHECK( result.toString(), QString( "65536" ) );

    result = QBigInteger( "4294967295" ) += QBigInteger( 1 );
    CHECK( result.toString(), QString( "4294967296" ) );

    result = QBigInteger( "18446744073709551615" ) += QBigInteger( 1 );
    CHECK( result.toString(), QString( "18446744073709551616" ) );

    result = QBigInteger( "124536363637272472" ) += QBigInteger( "124536363637272472" );
    CHECK( result.toString(), QString ( "249072727274544944" ) );

    result = QBigInteger( "9223372036854775807" ) += QBigInteger( "281474976710655" );
    CHECK( result.toString(), QString ( "9223653511831486462" ) );

    result = QBigInteger( "9223372036854775807" ) += QBigInteger( "137438953471" );
    CHECK( result.toString(), QString( "9223372174293729278" ) );

	   
    // Botan's carry tests
    result = QBigInteger( "340282366920938463463374607431768211455" )
	+= QBigInteger( "340282366920938463463374607431768211455" );
    CHECK( result.toString(), QString( "680564733841876926926749214863536422910" ) );

    result = QBigInteger( "340282366920938463463374607431768211455" )
	+= QBigInteger( "340282366920938463463374607431768211450" );
    CHECK( result.toString(), QString( "680564733841876926926749214863536422905" ) );

    result = QBigInteger( "115792089237316195423570985008687907853269984665640564039457584007913129639935" )
	+= QBigInteger( "115792089237316195423570985008687907853269984665640564039457584007913129639935" );
    CHECK( result.toString(), QString( "231584178474632390847141970017375815706539969331281128078915168015826259279870" ) );

    result = QBigInteger( "115792089237316195423570985008687907853269984665640564039457584007913129639935" )
	+= QBigInteger( "115792089237316195423570985008687907853269984665640564039457584007913129639919" );
    CHECK( result.toString(), QString( "231584178474632390847141970017375815706539969331281128078915168015826259279854") );

    result = QBigInteger( "13407807929942597099574024998205846127479365820592393377723561443721764030073546976801874298166903427690031858186486050853753882811946569946433649006084095" )
	+= QBigInteger( "18446744073709551616" );
    CHECK( result.toString(), QString( "13407807929942597099574024998205846127479365820592393377723561443721764030073546976801874298166903427690031858186486050853753882811946588393177722715635711" ) );


    result = QBigInteger( "13407807929942597099574024998205846127479365820592393377723561443721764030073546976801874298166903427690031858186486050853753882811946569946433649006084095" )
	+= QBigInteger( "1" );
    CHECK( result.toString(), QString( "13407807929942597099574024998205846127479365820592393377723561443721764030073546976801874298166903427690031858186486050853753882811946569946433649006084096" ) );

#if BUILD_WITH_NEGATIVE_CONSTRUCTOR
    result = QBigInteger( "-39794270013919406610834826960427146769766189764838473416502965291920535601112688579198627475284777498059330306128763345008528325994574657552726381901" )
	+= QBigInteger( "-342238655038" );
    CHECK( result.toString(), QString( "-39794270013919406610834826960427146769766189764838473416502965291920535601112688579198627475284777498059330306128763345008528325994574657894965036939" ) );

    result = QBigInteger( "25110291853498940831251897922987678157346336093292373576945426289097725034326735312448621015537884914" )
	+= QBigInteger( "-36551081154398645734533965739979697527373251608055056627686956281114038842935173436543461" );
    CHECK( result.toString(), QString( "25110291853462389750097499277253144191606356395765000325337371232470038078045621273605685842101341453") );
#endif
    result = QBigInteger( "27802650352" )
	+= QBigInteger( "660736146705288303126411072388564329913778942" );
    CHECK( result.toString(), QString( "660736146705288303126411072388564357716429294" ) );

#if  BUILD_WITH_NEGATIVE_CONSTRUCTOR
    result = QBigInteger( "-1348245899955041864800954463709881466231496038216683608715424566397833766910915722793041224478985289" )
	+= QBigInteger( "11517149522866182358565152643595266257020228597058539113114732218008332987904361457299261161227276764386173666571334749062651694592291882972" );
    CHECK( result.toString(), QString( "11517149522866182358565152643595266257018880351158584071249931263544623106438129961261044477618561339819775832804423833339858653367812897683" ) );

    result = QBigInteger( "-17540530441681616962868251635133601915039026254996886583618243914226325157426408929602625346567256761818" )
	+= QBigInteger( "865200427983527245206901810160356641402419461642082623179544681519016990" );
    CHECK( result.toString(), QString( "-17540530441681616962868251635132736714611042727751679681808083557584922737964766846979445801885737744828" ) );
#endif

    result = QBigInteger( "128844776074298261556398714096948603458177018275051329218555498374" )
	+= QBigInteger( "443816313829150876362052235134610603220548928107697961229953611873695276391917150913346479060246759720475193648" );
    CHECK( result.toString(), QString( "443816313829150876362052235134610603220548928236542737304251873430093990488865754371523497335298088939030692022" ) );

#if BUILD_WITH_NEGATIVE_CONSTRUCTOR
    result = QBigInteger( "1709484189262457846620911889502097055085989595277300243221975568275935717696463" )
	+= QBigInteger( "-1646592344139809206374540620411514484579951199941360" );
    CHECK( result.toString(), QString( "1709484189262457846620911887855504710946180388902759622810461083695984517755103" ) );

    result = QBigInteger( "320175865429637176165709341576187102540180627806418015204928771170233538951323952509055929139673223273528062883083030595199153877335714942842" )
	+= QBigInteger( "-2828241696960736089879965882386687935938570856545481227619497640844399275054327390050478930503975773972" );
    CHECK( result.toString(), QString( "320175865429637176165709341576187102537352386109457279115048805287846851015385381652510447912053725632683663608028703205148674946831739168870" ) );

    result = QBigInteger( "-4035398360542181725908295312107496142105415014744259439963377204111754181625695349185753326709217" )
	+= QBigInteger( "85450213703789913646546187382091037800" );
    CHECK( result.toString(), QString( "-4035398360542181725908295312107496142105415014744259439963291753898050391712048802998371235671417" ) );

    result = QBigInteger( "-1292166446073479876801522363382357887431657639184151284775525387363973852756087726243671676713861533673009088319851" )
	+= QBigInteger( "804538895874518175537499425282375058236245531798590350403343841766955572070643267141945695624895109330242749935754739434394691714971" );
    CHECK( result.toString(), QString( "804538895874518174245332979208895181434723168416232462971686202582804287295117879777971842868807383086571073221893205761385603395120" ) );

    result = QBigInteger( "-451986588700926309459451756852005697379481014956007968529234251884946522682901215022086432597024324062240835564200177389" )
	+= QBigInteger( "15762983479" );
    CHECK( result.toString(), QString( "-451986588700926309459451756852005697379481014956007968529234251884946522682901215022086432597024324062240835548437193910" ) );

    result = QBigInteger( "-3907475412115728816974567022055278374116794025624287474334038831885743634200801846649105209920908153587891040882946582394429615396962188674594744360388466" )
	+= QBigInteger( "193893611236537854694879677478106237157079207398283117392998175454362643521031390" );
    CHECK( result.toString(), QString( "-3907475412115728816974567022055278374116794025624287474334038831885743634006908235412567355226028476109784803725867374996146498003964013220232100839357076" ) );


    result = QBigInteger( "-72603710637966201224690926289" )
	+= QBigInteger( "-13618442642298533261581255034923612640512507150728017106768861506299813289801666559564532" );
    CHECK( result.toString(), QString( "-13618442642298533261581255034923612640512507150728017106768934110010451256002891250490821" ) );

    result = QBigInteger( "56077960835713056831402948406790747107889446769357509759472207603483968107693997028111823994257399379783658853302692762256851623103019589392739" )
	+= QBigInteger( "-427057313888431079237360487703561848638868677065083968842" );
    CHECK( result.toString(), QString( "56077960835713056831402948406790747107889446769357509759472207603483968107693997028111396936943510948704421492814989200408212754425954505423897" ) );

    result = QBigInteger( "-2209800838508504443494783762534800337712101405156784708782197580824527899758308" )
	+= QBigInteger( "42844076503039495864500213925837598507817708418354152774112078596443089606598570396235816327987463393971710495985285591895096794994387176281079" );
    CHECK( result.toString(), QString( "42844076503039495864500213925837598507817708418354152774112078594233288768090065952741032565452663056259609090828500883112899214169859276522771" ) );
#endif

    result = QBigInteger( "33887767308809826842417841176152232321272231788338404526859019370507113927387984766381329515371768224976188337692" )
	+= QBigInteger( "349484339542971517481628970179002500341" );
    CHECK( result.toString(), QString( "33887767308809826842417841176152232321272231788338404526859019370507113927737469105924301032853397195155190838033" ) );

#if  BUILD_WITH_NEGATIVE_CONSTRUCTOR
    result = QBigInteger( "85748089639858660722587321621536298082690707526412426951630101551228144063151688592419555048867068162" )
	+= QBigInteger( "-383634567691961960211191292397062452265352651123492760493087381707279" );
    CHECK( result.toString(), QString( "85748089639858660722587321621535914448123015564452215760337704488775878710500565099659061961485360883" ) );
#endif

    result = QBigInteger( "23889807888563742283608049816129153552608399262924421832404872043475" )
	+= QBigInteger( "995" );
    CHECK( result.toString(), QString( "23889807888563742283608049816129153552608399262924421832404872044470" ) );

#if BUILD_WITH_NEGATIVE_CONSTRUCTOR
    result = QBigInteger( "-654786925833474864669230962582694222611472680701859262466465606239654996048306783957549697781271829257774329538985" )
	+= QBigInteger( "-276137507159648540503039013089014674747" );
    CHECK( result.toString(), QString( "-654786925833474864669230962582694222611472680701859262466465606239654996048582921464709346321774868270863344213732" ) );

    result = QBigInteger( "50463316268089933" )
	+= QBigInteger( "-140591583463431806921000349498135287589005423318927850947894242995310138569473157521312413652439234324419130527702899917161307657443381774866237429" );
    CHECK( result.toString(), QString( "-140591583463431806921000349498135287589005423318927850947894242995310138569473157521312413652439234324419130527702899917161307657392918458598147496" ) );
#endif


    result = QBigInteger( "1339015021665554488163337105187026760232395594198925052890859936\
418304234254229440059229155546157793544192" )
	+= QBigInteger( "6294037420283433712414743361937677483761554699961644450461297486224793278823004487175687771163597590566132592591599249970281125781761944353272" );
    CHECK( result.toString(), QString( "6294037420283433712414743361937677485100569721627198938624634591411820039055400081374612824054457526984436826845828690029510281327919737897464" ) );

#if BUILD_WITH_NEGATIVE_CONSTRUCTOR
    result = QBigInteger( "-241446683" )
	+= QBigInteger( "-282671163032866994488211995758272717472259277760825940523445628\
442206062910449311538519756165635175664610569214430918184214" );
    CHECK( result.toString(), QString( "-282671163032866994488211995758272717472259277760825940523445628442206062910449311538519756165635175664610569214431159630897" ) );

    result = QBigInteger( "2358605503303452637996081421902056515951744611718383128442445119505739707550326378912342448355046239066896995563581" )
	+= QBigInteger( "-3830437229145325165273364525551261440648845791949681661260946956860463720730123941973615" );
    CHECK( result.toString(), QString( "2358605503303452637996081418071619286806419446445018602891183678856893915600644717651395491494582518336773053589966" ) );

    result = QBigInteger( "1860794367587960058388097846258490" )
	+= QBigInteger( "-237344494507203983863096991896035366478949095337787603280" );
    CHECK( result.toString(), QString( "-237344494507203983863095131101667778518890707239941344790" ) );

    result = QBigInteger( "-286399096802321907543674770412181810379003627366516307780436082546" )
	+= QBigInteger( "6433131620680089024037442172197761714707480582555136398379812339597187475099646442833150194" );
    CHECK( result.toString(), QString( "6433131620680089024037441885798664912385573038880365986198001960593560108583338662397067648" ) );

    result = QBigInteger( "81180339077102369559537817583627894783322804181859729574752442572146800569023773490164987520541203125338295785763244283224569259250011493" )
	+= QBigInteger( "-1199127665773503170250307078028035875479459397657178356959526245067549497129923023348187933280753018204983010837846725666878521137637491" );
    CHECK( result.toString(), QString( "179981211411328866389287510505599858907843344784202551217792916327079251071893850466816799587260450107133312774925397557557690738112374002" ) );

    result = QBigInteger( "-64140201395555533811408642891620184652051275811075926176282032144915585503450776768366775652419022149512034611311149858695307750874152" )
	+= QBigInteger( "174441039" );
    CHECK( result.toString(), QString( "-64140201395555533811408642891620184652051275811075926176282032144915585503450776768366775652419022149512034611311149858695307576433113" ) );
#endif

    result = QBigInteger( "1272757944308835857208037878018507337530557445422230495561634616503724419877512717512360239259640193513601352202821462208896049331599624285621" )
	+= QBigInteger( "7326562354017884140300121264633612334070903165496641915889499701\
38457507491850467631029977010" );
    CHECK( result.toString(), QString( "1272757944308835857208037878018507337530557445422963151797036404917754432003976078745767329576189857705190302172959919716387899799230654262631" ) );

#if BUILD_WITH_NEGATIVE_CONSTRUCTOR
    result = QBigInteger( "-296171972628230" )
	+= QBigInteger( "-8295766099121843219000823699362222865173820102569731517716391727126741710202086962877467940292139" );
    CHECK( result.toString(), QString( "-8295766099121843219000823699362222865173820102569731517716391727126741710202086963173639912920369" ) );

    result = QBigInteger( "746985914068199510024843682108839444828414222769191520615967632362127522466922882591" )
	+= QBigInteger( "-20487191102299831461877807785745372724903547246374023" );
    CHECK( result.toString(), QString( "746985914068199510024843682108818957637311922937729642808181886989402618919676508568" ) );

    result = QBigInteger( "-4" )
	+= QBigInteger( "-2344390090753264806043234960981151613122271366762590006930318876906455201397017135" );
    CHECK( result.toString(), QString( "-2344390090753264806043234960981151613122271366762590006930318876906455201397017139" ) );

    result = QBigInteger( "-44876180273995737337769331875058141129678736711749946388832275767882143882764" )
	+= QBigInteger( "20982187786" );
    CHECK( result.toString(), QString( "-44876180273995737337769331875058141129678736711749946388832275767861161694978" ) );

    result = QBigInteger( "-6019440082648243511340058232981487443695615379104154368957939907896782179207195666302228625496897271988494" )
	+= QBigInteger( "532566302499155416003316607801593784583652720754079760364736422291735917382015688217276924340984564880" );
    CHECK( result.toString(), QString( "-6018907516345744355924054916373685849911031726383400289197575171474490443289813650614011348572556287423614" ) );

    result = QBigInteger( "-73755471563616026847726349357167530833850959662921059052928229237814728719448868719278211294785998253117976812683153264088230182865250970217610487" )
	+= QBigInteger( "-30100016097092378349958946184353117306134810372681" );
    CHECK( result.toString(), QString( "-73755471563616026847726349357167530833850959662921059052928229237814728719448868719278211294786028353134073905061503223034414535982557105027983168" ) );

    result = QBigInteger( "-2211177066689704345686852756638946306674958952044447080285364283965878599873864667094550865713828159912" )
	+= QBigInteger( "-5365560439372456892007565798761606781997269201538475736814780300517383963455858081652308237033460360040921820049494698892905680307378540208" ); 
    CHECK( result.toString(), QString( "-5365560439372456892007565798761606784208446268228180082501633057156330270130817033696755317318824644006800419923359365987456546021206700120" ) );
#endif

    result = QBigInteger( "6074122512337108841968521649035076841633691574254417104144285970819068715158037023149867252146570418484850234979838064249373816163440" )
	+= QBigInteger( "301843614094506325875637699" );
    CHECK( result.toString(), QString( "6074122512337108841968521649035076841633691574254417104144285970819068715158037023149867252146570418484850536823452158755699691801139" ) );

#if BUILD_WITH_NEGATIVE_CONSTRUCTOR
    result = QBigInteger( "-518214776931158149908771340564348982010543985108065053479219152734659892042499774128809654713651547833087206893256740737426200715673766732196603988" )
	+= QBigInteger( "-29835172557747693726115525887386137004674545311422557345658884038760353928226157702249175218280718951979" );
    CHECK( result.toString(), QString( "-518214776931158149908771340564348982010544014943237611226912878850185779428636778803354966136208893491971245653610668963583902964848985012915555967" ) );
#endif

    result = QBigInteger( "15937412249227240968245047444122" )
	+= QBigInteger( "186214680376169426108822450700978827886569053440254258585576645530381613666540347032550716844628275956253" );
    CHECK( result.toString(), QString( "186214680376169426108822450700978827886569053440254258585576645530381613682477759281777957812873323400375" ) );

#if BUILD_WITH_NEGATIVE_CONSTRUCTOR
    result = QBigInteger( "-12528010116258685855047504252928107623923105458701761707911969527003855713485846140551107967495813584097081777160" )
	+= QBigInteger( "-539986280927242338236008809854961759996986302156061552378097160849129372827386927545686899193598721998757419572890" );
    CHECK( result.toString(), QString( "-552514291043501024091056314107889867620909407614763314086009130376133228540872773686238007161094535582854501350050" ) );

    result = QBigInteger( "-2454746908" )
	+= QBigInteger( "-3822957127889394780055242156360370187075592078655552376050604679934415014573879513870030211860839641756441626913419699098985245833920954444218" );
    CHECK( result.toString(), QString( "-3822957127889394780055242156360370187075592078655552376050604679934415014573879513870030211860839641756441626913419699098985245833923409191126" ) );


    result = QBigInteger( "-54288706131860071583318409080596095357980447323635" )
	+= QBigInteger( "-425339410556015631098973742993327323051438456819027069606294261157940297643297240559452124432779202181589763874" );
    CHECK( result.toString(), QString( "-425339410556015631098973742993327323051438456819027069606294315446646429503368823877861205028874560162037087509" ) );

    result = QBigInteger( "1418766894051319870818496026367686195459604395660119754151922014257535705077512233275240217434104" )
	+= QBigInteger( "-111987390206074845527" );
    CHECK( result.toString(), QString( "1418766894051319870818496026367686195459604395660119754151922014257535705077400245885034142588577" ) );

    result = QBigInteger( "-690410131860410477456103857594543515409677479242833618634809302452962600476353286822550168231234854116465153078845744722987447719420052500874721214723" )
	+= QBigInteger( "-2584690377433946747311356992432788361455494791066739384837409609897387109736539600623155880918146331681272708396146283818299" );
    CHECK( result.toString(), QString( "-690410131860410477456103860179233892843624226554190611067597663908457391543092671659959778128621963853004753702001625641133779400692760897021005033022" ) );

    result = QBigInteger( "-2326153002179462643778624079324592172489363679671158" )
	+= QBigInteger( "-109819757548464054181938329012610459679" );
    CHECK( result.toString(), QString( "-2326153002179572463536172543378774110818376290130837" ) );

    result = QBigInteger( "-4428752250566525488353857709194941742993785578807911414016959206453045495320705299466107784149485981354180907411034982168391" )
	+= QBigInteger( "-39247778259374215325521768005388007526581235832446540589720560855741992694947322437679214611686905696" );
    CHECK( result.toString(), QString( "-442875225056652548835389695697320111720911110057591680202448578\
7688877941861295020026963526142180928676618586625646669074087" ) );

    result = QBigInteger( "3047" )
	+= QBigInteger( "-73564587850313153523776932163719610733433776890390204618040173797196000100856070829277943048343156165795282307508135277641315214" );
    CHECK( result.toString(), QString( "-73564587850313153523776932163719610733433776890390204618040173797196000100856070829277943048343156165795282307508135277641312167" ) );

    result = QBigInteger( "71397189765381049110362731262243394989390499523719445987286843598407339615555456955143712741779487184644001767776382991377987516772847242986" )
	+= QBigInteger( "-5821969555717973232123574849275726788359152255219972775831" );
    CHECK( result.toString(), QString( "71397189765381049110362731262243394989390499523719445987286843598407339615555456949321743186061513952520426918500656203018835261552874467155" ) );

    result = QBigInteger( "-181409752656613138777964092635909379021826360390960647186726991165227400176766831466541160049935205507919070233410228328274" )
	+= QBigInteger( "-523301382154855044703947051892202646490840761177533623732372519689918420769842424772676407501350528096714904915297347684247802773107355881667545916901" );
    CHECK( result.toString(), QString( "-523301382154855044703947052073612399147453899955497716368281898711744781160803071959403398666577928273481736381838507734183008281026426115077774245175" ) );

    result = QBigInteger( "6858961373707073067" )
	+= QBigInteger( "-334051508933893061433844279764271107181974906283364991309903077649971606436918071327072869826471946094594594115614990907" );
    CHECK( result.toString(), QString( "-334051508933893061433844279764271107181974906283364991309903077649971606436918071327072869826471946087735632741907917840" ) );

    result = QBigInteger( "-23635098930374569407171906960429616870908424281519944658490940109956689534874971218650241680916564611" )
	+= QBigInteger( "-18958917875779522833599589133142827952448539301142718746979271443846670235982743793439686626736428198541647202983677887505430060922528525205" );
    CHECK( result.toString(), QString( "-18958917875779522833599589133142827952472174400073093316386443350807099852853652217721206571394919138651603892518552858724080302603445089816" ) );
#endif 

    // Botan's subtraction tests
    result = QBigInteger( "0" )
	-= QBigInteger( "0" );
    CHECK( result.toString(), QString( "0" ) );

    result = QBigInteger( "0" )
	-= QBigInteger( "1" );
    CHECK( result.toString(), QString( "-1" ) );

    result = QBigInteger( "0" )
	-= QBigInteger( "4294967296" );
    CHECK( result.toString(), QString( "-4294967296" ) );

    // Next test is labelled # 2^512 - 1
    result = QBigInteger( "13407807929942597099574024998205846127479365820592393377723561443721764030073546976801874298166903427690031858186486050853753882811946569946433649006084095" )
      -= QBigInteger( "1" );
    CHECK( result.toString(), QString( "13407807929942597099574024998205846127479365820592393377723561443721764030073546976801874298166903427690031858186486050853753882811946569946433649006084094" ) );

    result = QBigInteger( "89094716573076464980713547115099137014719483620102078148320806773871083148864" )
	-= QBigInteger( "49505213825110728957828173754776257356620450607893971553289366249708672306581" );
    CHECK( result.toString(), QString( "39589502747965736022885373360322879658099033012208106595031440524162410842283" ) );
    
    result = QBigInteger( "65894747009896006767807716946835412110318548717263922395390971078905789585431" )
	-= QBigInteger( "3884269741925508225990715416862047284194603799902650748631039608684367281358" );
    CHECK( result.toString(), QString( "62010477267970498541817001529973364826123944917361271646759931470221422304073" ) );
    
    result = QBigInteger( "5950196396451977566902121301707054218364717196893101360011491777761952253736964709165962613347710607164178682987783755894811024288429224592316636383" )
	-= QBigInteger( "8750653273562160761286422180115618621879821429145276197424652349306577311499807887070429373153777028581165316131683348567" );
    CHECK( result.toString(), QString( "5950196396451977566902121292956400944802556435606679179895873155882130824591688511741310264041133295664370795917354382741033995707263908460633287816" ) );
    
    result = QBigInteger( "9815262808265519920770782360080149146267723690" )
	-= QBigInteger( "14067005768891609281364919358115291341352189918255780397560060748765650205261663193732434161580120817" );
    CHECK( result.toString(), QString( "-14067005768891609281364919358115291341352189918255780387744797940500130284490880833652285015312397127" ) );

#if BUILD_WITH_NEGATIVE_CONSTRUCTOR    
    result = QBigInteger( "-390149102941948621568479722346940666704376013734485343840154221605853412503154993878886490867020934777530894593416337175399865065870417717658815158195790" )
	-= QBigInteger( "1456031684988128870809574635750149625240648487837308" );
    CHECK( result.toString(), QString( "-390149102941948621568479722346940666704376013734485343840154221605853412503154993878886490867020934778986926278404466046209439701620567342899463646033098" ) );


    result = QBigInteger( "7473774301764883450943" )
	-= QBigInteger( "-26256369859367890755157372820052387483402723790185562908491933812453" );
    CHECK( result.toString(), QString( "26256369859367890755157372820052387483402723797659337210256817263396" ) );
#endif
    
    result = QBigInteger( "36246343251214922024139186757009148849295485593397952003237349660142296147421019916619944353877490544706223768684758263065399016597969" )
	-= QBigInteger( "2574427901445527995149185461475228850098549655325125750771680756403104624569522792792597223218143154924988199562355517064962665954307425375180" );
    CHECK( result.toString(), QString( "-2574427865199184743934263437336042093089400806029640157373728753165754964427226645371577306598198801047497654856131748380204402888908408777211" ) );
    
#if BUILD_WITH_NEGATIVE_CONSTRUCTOR
    result = QBigInteger( "30129746266682790628283889040897642317014108334116727" )
	-= QBigInteger( "-1580480523895398762563721715474380903630073871362143915864398724834897608423" );
    CHECK( result.toString(), QString( "1580480523895398762563751845220647586420702155251184813506715738943231725150" ) );
    
    result = QBigInteger( "-4614735863800137951667138933166372061" )
	-= QBigInteger( "87175694379075561307234146162193190462135078700346746992273" );
    CHECK( result.toString(), QString( "-87175694379075561307238760898056990600086745839279913364334" ) );
    
    result = QBigInteger( "-3753904" )
	-= QBigInteger( "-11269137783745339515071988205310702154422777729974" );
    CHECK( result.toString(), QString( "11269137783745339515071988205310702154422773976070" ) );
    
    result = QBigInteger( "592523948495379440082021279738170088402918858455470050140652787171830058864932939900794505955437856926902975870288" )
	-= QBigInteger( "-205854658295495452479104108497931263758143158076949293929661651111" );
    CHECK( result.toString(), QString( "592523948495379440082021279738170088402918858455675904798948282624309162973430871164552649113514806220832637521399" ) );
    
    result = QBigInteger( "-33993701617495591491176844355" )
	-= QBigInteger( "3438065097398894672826284379125235190693300918673662774192379185002391232383325160416036963599856704698280" );
    CHECK( result.toString(), QString( "-3438065097398894672826284379125235190693300918673662774192379185002391232383359154117654459191347881542635" ) );
    
    result = QBigInteger( "26876428790838270949718735111909136008255051776703" )
	-= QBigInteger( "-1781128112966810373286192008831149275546995635268767241859967609117529616872536681035700534316457543887601645022" );
    CHECK( result.toString(), QString( "1781128112966810373286192008831149275546995635268767241859967635993958407710807630754435646225593552142653421725" ) );
#endif

    result = QBigInteger( "2059771092932179758019770618974659367350250375647433386639519387\
69317693429941871882153770641334267205446421916220398066553188" )
	-= QBigInteger( "3342500267594994347156312297990633112620923791590960237694328174171473763026" );
    CHECK( result.toString(), QString( "205977109293217975801977061897465936735025037564739996163684343774970537117643881249041149717542676245208727588046226592790162" ) );
    
    result = QBigInteger( "5545520403000578843599072515870982842927227412121917598877293331575380404618111609" )
	-= QBigInteger( "5991287327241003718821424770352575362437680738923552868139860461945460339860477495902" );
    CHECK( result.toString(), QString( "-5985741806838003139977825697836704379594753511511430950540983168613884959455859384293" ) );
    
    result = QBigInteger( "248039029608125071340" )
	-= QBigInteger( "3664608673" );
    CHECK( result.toString(), QString( "248039029604460462667" ) );

#if BUILD_WITH_NEGATIVE_CONSTRUCTOR    
    result = QBigInteger( "15425705711415937103627" )
	-= QBigInteger( "-1435504065517745703440045276868982910754081405474123003767554211132837427846963435621523810229738262235546179779885824" );
    CHECK( result.toString(), QString( "1435504065517745703440045276868982910754081405474123003767554211132837427846963435621523810229753687941257595716989451" ) );
#endif

    result = QBigInteger( "50882847205108645607281568922683652688671738236030732914347600821086" )
	-= QBigInteger( "12176160963158" );
    CHECK( result.toString(), QString( "50882847205108645607281568922683652688671738236030732902171439857928" ) );

#if BUILD_WITH_NEGATIVE_CONSTRUCTOR    
    result = QBigInteger( "-35426518565985818947670047877033022885542172461973566228509771053416312543201815881190953762207629232160412058300173038824256783171761132" )
	-= QBigInteger( "-4864862607366468843184694353123830534588538011093812418208808135799" );
    CHECK( result.toString(), QString( "-35426518565985818947670047877033022885542172461973566228509771053416307678339208514722110577513276108329877469762161945011838574363625333" ) );

    result = QBigInteger( "-1428596214712268310382144828171384812520179141608121870013556402879770424002218157546599921571184" )
	-= QBigInteger( "-4054101" );
    CHECK( result.toString(), QString( "-1428596214712268310382144828171384812520179141608121870013556402879770424002218157546599917517083" ) );

    result = QBigInteger( "-200931" )
	-= QBigInteger( "-44558802460130495759482832913160717791151786725570519475449607659705171682283111490834930835045735142966847483009157514950177565952218520297258834187372" );
    CHECK( result.toString(), QString( "44558802460130495759482832913160717791151786725570519475449607659705171682283111490834930835045735142966847483009157514950177565952218520297258833986441" ) );

    result = QBigInteger( "105704314890799915321259" )
	-= QBigInteger( "827923545945076415574912438499169814414563066877494100831657761190490697473854369477784874118787495351405549803329615347120938123226038208" );
    CHECK( result.toString(), QString( "-827923545945076415574912438499169814414563066877494100831657761190490697473854369477784874118787495351405549803329509642806047323310716949" ) );

    result = QBigInteger( "1448979433940064018828919290452280235308901982649341" )
	-= QBigInteger( "303926827425887072291878308433008512899006711759770318009" );
    CHECK( result.toString(), QString( "-303925378446453132227859479513718060618771402857787668668" ) );

    result = QBigInteger( "-243237595290235750457450892290434789864" )
	-= QBigInteger( "19817702076334276402981273067417321098467533300947463865383702005126562800253466403934608765512316565811954342319565128573969" );
    CHECK( result.toString(), QString( "-19817702076334276402981273067417321098467533300947463865383702005126562800253466403934852003107606801562411793211855563363833" ) );
#endif

    result = QBigInteger( "294037338365659932242802023634" )
	-= QBigInteger( "4401245995535867764294876849802142926077599828776505639975554254356763769548465" );
    CHECK( result.toString(), QString( "-4401245995535867764294876849802142926077599828776211602637188594424520967524831" ) );

    result = QBigInteger( "7303853946195223307036710881687367004566538357189824031021831088365362" )
	-= QBigInteger( "119286025999378935715794641163321741" );
    CHECK( result.toString(), QString( "7303853946195223307036710881687366885280512357810888315227189925043621" ) );

    result = QBigInteger( "571167355343287235687602610714110416067426289363505412908804940696550592413192300554016875" )
	-= QBigInteger( "15872188842802631759540597" );
    CHECK( result.toString(), QString( "5711673553432872356876026107141104160674262893635054129088049406\
80678403570389668794476278" ) );

}

