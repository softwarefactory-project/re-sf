[@decco]
type service = {
  name: string,
  path: string,
};

[@decco]
type link = {
  name: string,
  link: string,
};

[@decco]
type auth_other = {
  name: string,
  text: string,
};

[@decco]
type auth = {
  oauth: list(string),
  other: list(auth_other),
};

[@decco]
type t = {
  services: list(service),
  auths: auth,
  version: string,
  documentation_links: list(link),
  status_links: list(link),
  contact_links: list(link),
  header_logo_b64data: string,
  splash_image_b64data: string,
};

let encode = t_encode;
let decode = t_decode;
