let example = {|
  {
    "default-connection": "rdoproject.org",
    "description": "The rdoproject.org tenant",
    "tenant-options": {
        "zuul/report-build-page": true,
        "zuul/web-root": "https://review.rdoproject.org/zuul/"
    },
    "url": "https://review.rdoproject.org/manage",
    "name": "rdoproject.org"
  }
|};

type tenant = {
  name: string,
  default_connection: option(string),
  description: option(string),
  url: string,
};

let parse = json => {
  Json.Decode.{
    name: json |> field("name", string),
    default_connection: json |> optional(field("default-connection", string)),
    url: json |> field("url", string),
    description: json |> optional(field("description", string)),
  };
};

let runExample = () => {
  Js.log("Running tenant example");
  let json = Json.parseOrRaise(example);
  let connections = parse(json);
  Js.log(connections);
};
