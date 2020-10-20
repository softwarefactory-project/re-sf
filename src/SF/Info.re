// A software factory info context
type info = {
  services: list(service),
  version: string,
  header_logo_b64data: string,
  splash_image_b64data: string
}
and service = {
  name: string,
  path: string,
};

let parseService = (json): service =>
  Json.Decode.{
    name: json |> field("name", string),
    path: json |> field("path", string),
  };

let parse = (json): info =>
  Json.Decode.{
    version: json |> field("version", string),
    header_logo_b64data: json |> field("header_logo_b64data", string),
    splash_image_b64data: json |> field("splash_image_b64data", string),
    services: json |> field("services", list(parseService)),
  };
