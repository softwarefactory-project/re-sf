module SFV1 = SF;

let mapFilterMap = (x, f, g) =>
  x
  ->Belt.Option.map(y => y->Belt.List.keep(f)->Belt.List.map(g))
  ->Belt.Option.getWithDefault([]);

let mapMap = (x, f) => mapFilterMap(x, _ => true, f);

module Connection = {
  [@decco]
  type local = {acl: string};
  [@decco]
  type pagure = {url: string};
  [@decco]
  type t =
    | Local(local)
    | Pagure(pagure)
    | Todo(string)
    | Unknown(string)
    | Unspecified;

  let fromV1 = (connections: list(Connection.t), name: string): t => {
    switch (connections->Belt.List.keep(c => c.name == name)->Belt.List.head) {
    | Some(c) =>
      switch (c.connection_type) {
      | SFV1.Connection.Pagure =>
        Pagure({url: c.base_url->Belt.Option.getExn})
      | _ => Todo(c.name)
      }
    | None => name != "" ? Unknown(name) : Unspecified
    };
  };
};

let getLocal = (res: SFV1.Resources.t, name: string): option(SFV1.Repo.t) =>
  res.repos
  ->Belt.Option.flatMap(repos =>
      repos->Belt.List.keep(r => r.name == name)->Belt.List.head
    );

module SourceRepository = {
  [@decco]
  type t = {
    name: string,
    description: option(string),
    location: Connection.t,
  };
  let fromV1 =
      (
        res: Resources.t,
        project: SFV1.Project.t,
        tenant: SFV1.Tenant.t,
        sr: SFV1.SourceRepository.union,
      )
      : t => {
    let name = SFV1.SourceRepository.getName(sr);
    let isLocal = getLocal(res, name);
    let connectionName =
      SFV1.SourceRepository.getConnection(sr)
      ->Belt.Option.getWithDefault(
          project.connection
          ->Belt.Option.getWithDefault(
              tenant.default_connection->Belt.Option.getWithDefault(""),
            ),
        );
    let description = isLocal->Belt.Option.map(r => r.description);
    let location =
      switch (isLocal) {
      | Some(repo) => Connection.Local({acl: repo.acl})
      | None =>
        Connection.fromV1(
          res.connections->Belt.Option.getWithDefault([]),
          connectionName,
        )
      };
    {name, description, location};
  };
};

module Project = {
  [@decco]
  type t = {
    name: string,
    description: string,
    [@decco.key "source-repositories"]
    source_repositories: list(SourceRepository.t),
    website: option(string),
    documentation: option(string),
    [@decco.key "issue-tracker-url"]
    issue_tracker_url: option(string),
    [@decco.key "mailing-lists"]
    mailing_lists: option(list(string)),
    contacts: option(list(string)),
  };
  let fromV1 =
      (res: Resources.t, tenant: SFV1.Tenant.t, project: SFV1.Project.t): t => {
    name: project.name,
    description: project.description,
    source_repositories:
      project.sourceRepositories
      ->Belt.List.map(SourceRepository.fromV1(res, project, tenant)),
    website: project.website,
    documentation: project.documentation,
    issue_tracker_url: project.issue_tracker_url,
    mailing_lists: project.mailing_lists,
    contacts: project.contacts,
  };
};

module Tenant = {
  [@decco]
  type t = {
    name: string,
    projects: list(Project.t),
  };
  let fromV1 =
      (defaultTenant: string, res: Resources.t, tenant: SFV1.Tenant.t): t => {
    let isMyProject = (project: SFV1.Project.t): bool =>
      switch (project.tenant) {
      // This project has a tenant name, so check if it is the tenant we are loading
      | Some(projectTenant) => tenant.name == projectTenant
      // This project does not have a tenant, so check the current tenant is the default
      | None => tenant.name == defaultTenant
      };
    {
      name: tenant.name,
      projects:
        res.projects->mapFilterMap(isMyProject, Project.fromV1(res, tenant)),
    };
  };
};

[@decco]
type t = {tenants: list(Tenant.t)};

let fromV1 = (~defaultTenant: string, res: Resources.t): t => {
  {tenants: res.tenants->mapMap(Tenant.fromV1(defaultTenant, res))};
};

let toV1 = (_v2: t): Resources.t => {
  tenants: None,
  projects: None,
  connections: None,
  repos: None,
  groups: None,
};
