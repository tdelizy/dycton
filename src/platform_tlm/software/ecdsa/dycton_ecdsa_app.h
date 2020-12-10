#ifndef DYCTON_ECDSA_APP_H
#define DYCTON_ECDSA_APP_H

#define DYCTON_RUN

#define ECDSA_IN_FILE_ADDR       	(DENSE_MEM_BASE)
#define ECDSA_IN_FILE_SZ       	  	(1048576) // in bytes
#define SEED_FILE_PATH_IN           ("../software/ecdsa/test.seed")

#define DATASET_COUNT 		(8)


const char * input_0 = "test test test test test more test total test.";
const char * input_1 = "04999599488945829582945724905929458072495872045892049587204985709248570294857847584758475928491830598709283547950918347";
const char * input_2 = "As with elliptic-curve cryptography in general, the bit size of the public key believed to be needed for ECDSA is about twice the size of the security level, in bits. For example, at a security level of 80 bits (meaning an attacker requires a maximum of about 2^80 operations to find the private key) the size of an ECDSA public key would be 160 bits, whereas the size of a DSA public key is at least 1024 bits. On the other hand, the signature size is the same for both DSA and ECDSA: approximately 4t bits, where t is the security level measured in bits, that is, about 320 bits for a security level of 80 bits.";
const char * input_3 = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
const char * input_4 = "L'apparition de la tarte flambée de la région du Kochersberg7,8, au nord de Strasbourg et de l'Alsace, aux portes de la Lorraine et de l'Allemagne, remonte à l’habitude des paysans des siècles précédents, de faire cuire leur pain dans des fours à pain au feu de bois, toutes les deux ou trois semaines. C’était alors une petite fête et, pour la marquer, la pâte qui restait après la préparation des miches était étalée et recouverte de lait caillé « sürmellich » (plus tard de fromage blanc, d'oignons et de lardons) et le tout était passé pendant quelques minutes au four. Les tartes flambée était alors présentée sur une planche à découper en bois de boulanger, et était découpée en rectangles. Après avoir rapidement détaché les bords un peu noircis, chacun roulait sa part et la mangeait brûlante avec les doigts. C'est d'ailleurs toujours ainsi que les Alsaciens la mangent, même au restaurant : avec les doigts. La cuisson traditionnelle du pain dans un four à bois en pierre se prêtait à la cuisson de ces flammekueche : immédiatement après le chauffage du four au feu de bois, celui-ci était trop chaud pour y faire cuire le pain. Il fallait attendre qu'il atteigne la température correcte. La température était à ce moment idéale pour faire cuire les tartes flambées. On repoussait les braises, on glissait la fine tarte, et en 2 à 3 minutes, elle était prête lorsque ses bords léchés par les flammes commençaient à noircir.Contrairement à l'idée qu'on peut se faire de ce plat typique, la tarte flambée n'est apparue dans les restaurants de Strasbourg qu'à la fin des années 1960, après la mode des pizzerias. Les premiers qui régalaient cette nouvelle spécialité, étaient des restaurants de campagne, notamment du Kochersberg, qui n'ouvraient que les samedis et dimanches soir, la tarte flambée étant un plat simple généralement fait à la maison.L'essor du tourisme a popularisé ce plat et, aujourd’hui, de nombreux établissements et Winstubs le proposent non seulement dans le Kochersberg, qui passe pour le berceau de la flammekueche, mais aussi dans toute l’Alsace (parfois agrémentée de munster dans le Haut-Rhin) et aussi de l'autre côté du Rhin, dans le pays de Bade, ainsi qu'en Moselle germanophone où la base n'est pas du fromage blanc mais de la crème. Des chaînes de restaurants se sont spécialisées autour de ce produit. Dans la restauration et dans l'industrie alimentaire, le terme flammekueche s'est généralisé dès les années 1990, tandis que l'appellation « tarte flambée », trop ambiguë, tombait en désuétude (en dehors de l'Alsace où elle reste la plus usitée).";
const char * input_5 = "short length test";
const char * input_6 = "Les actions proposées se positionnent selon un axe croissant de maturité technologique mais aussi d’intégration plus ou moins renforcée de ces partenariats intersectoriels.";
const char * input_7 = "Une sous-partie de l’instrument de co-financement P4, dont l’objectif s’attache à régulariser le lien partenariat-entreprise, s’appuie sur une vision trilatérale de l’innovation technologique, du mécénat et de la recherche structurelle de pointe";


const char ** dataset_array[DATASET_COUNT] = {	&input_0, 
												&input_1,
												&input_2,
												&input_3,
												&input_4,
												&input_5,
												&input_6,
												&input_7};

// stub for sc_main.cpp
uint32_t dataset_in_size[DATASET_COUNT] = {	0,
											0,
											0,
											0,
											0,
											0,
											0,
											0};

											
uint32_t dataset_footprint[DATASET_COUNT] = {	7476, 
												7580,
												7428,
												7644,
												7596,
												7508,
												7524,
												7476};


#endif //DYCTON_ECDSA_APP_H











